#include "DynamicComboAttack.h"
#include "ComboAttackRoot.h"
#include "Frame/Frame.h"
#include "GameCamera/GameCamera.h"
#include "MathFunction.h"
#include "Player/Player.h"

DynamicComboAttack::DynamicComboAttack(Player* player, PlayerComboAttackData* attackData)
    : BaseComboAattackBehavior(attackData->GetGroupName(), player) {

    attackData_ = attackData;
    Init();
}

DynamicComboAttack::~DynamicComboAttack() {}

void DynamicComboAttack::Init() {

    // アニメーション初期化
    BaseComboAattackBehavior::AnimationInit();

    // タイミングのリセット
    currentFrame_      = 0.0f;
    waitTime_          = 0.0f;
    isCollisionActive_ = false;
    collisionTimer_    = 0.0f;

    // 移動イージングの初期化
    const Vector3& moveVector = pPlayer_->GetTransform().GetForwardVector() * attackData_->GetAttackParam().moveParam.value;

    startPosition_  = pPlayer_->GetWorldPosition();
    targetPosition_ = startPosition_ + moveVector;

    // moveParam取得
    const PlayerComboAttackData::MoveParam& moveParam = attackData_->GetAttackParam().moveParam;

    // イージングのセットアップ
    moveEasing_.SetType(static_cast<EasingType>(moveParam.easeType));
    moveEasing_.SetStartValue(startPosition_);
    moveEasing_.SetEndValue(targetPosition_);
    moveEasing_.SetAdaptValue(&currentMoveValue_);
   
    moveEasing_.SetOnFinishCallback([this]() {
        order_ = Order::RECOVERY;
    });

    //// サウンド再生
    //pPlayer_->SoundPunch();

    order_ = Order::INIT;
}

void DynamicComboAttack::Update() {
    // モーション更新
    BaseComboAattackBehavior::RotateMotionUpdate(0, GetRotateValue(), true);
    BaseComboAattackBehavior::FloatAnimationUpdate();
    BaseComboAattackBehavior::ScalingEaseUpdate();

    // 通常移動
    pPlayer_->Move(pPlayerParameter_->GetParamaters().moveSpeed);

    switch (order_) {
    case Order::INIT:
        InitializeAttack();
        break;

    case Order::ATTACK:
        UpdateAttack();
        break;

    case Order::RECOVERY:
        UpdateRecovery();
        break;

    case Order::WAIT:
        UpdateWait();
        break;
    }

    currentFrame_ += atkSpeed_;
}

void DynamicComboAttack::InitializeAttack() {
    // カメラアニメーション再生
    if (pPlayer_->GetGameCamera()) {
        // pPlayer_->GetGameCamera()->PlayAnimation("ComboAttack");
    }

    SetupCollision();
    order_ = Order::ATTACK;
}

void DynamicComboAttack::UpdateAttack() {
    // 移動適用
    ApplyMovement();

    // コリジョン判定
    auto& collisionParam = attackData_->GetAttackParam().collisionPara;

    collisionTimer_ += atkSpeed_;

    if (collisionTimer_ >= collisionParam.adaptTime) {
        // コリジョンを無効化
        isCollisionActive_ = false;
        pPlayer_->GetPlayerCollisionInfo()->SetIsCollision(false);
    } else {
        isCollisionActive_ = true;

        // プレイヤーの向きを取得
        float playerRotationY    = pPlayer_->GetTransform().rotation_.y;
        Matrix4x4 rotationMatrix = MakeRotateYMatrix(playerRotationY);

        // オフセットをワールド座標に変換
        Vector3 worldOffset  = TransformNormal(collisionParam.offsetPos, rotationMatrix);
        Vector3 collisionPos = pPlayer_->GetWorldPosition() + worldOffset;

        // コリジョン位置更新
        pPlayer_->GetPlayerCollisionInfo()->SetPosition(collisionPos);
    }

    // イージングが完了したらRecoveryへ
    if (moveEasing_.IsFinished()) {
        order_ = Order::RECOVERY;
    }
}

void DynamicComboAttack::UpdateRecovery() {
    pPlayer_->AdaptRotate();

    // 硬直時間
    static float recoveryTime = 0.0f;
    recoveryTime += atkSpeed_;

    if (recoveryTime >= 0.2f) { // 0.2秒の硬直
        recoveryTime = 0.0f;
        order_       = Order::WAIT;
    }
}

void DynamicComboAttack::UpdateWait() {
    waitTime_ += atkSpeed_;
    pPlayer_->AdaptRotate();

    auto& timingParam = attackData_->GetAttackParam().timingParam;

    // 次の攻撃への受付時間チェック
    if (waitTime_ >= timingParam.cancelFrame) {
        // コンボ途切れ
        pPlayer_->ChangeComboBehavior(std::make_unique<ComboAttackRoot>(pPlayer_));
    } else {
        // 先行入力受付
        if (waitTime_ >= timingParam.precedeInputFrame) {
            BaseComboAattackBehavior::PreOderNextComboForButton();

            if (isNextCombo_) {
                // 次の攻撃データを取得
                auto& nextAttackType = attackData_->GetAttackParam().nextAttackType;

                if (nextAttackType != "None" && !nextAttackType.empty()) {
                    // ComboAttackControllerから次の攻撃データを取得
                    auto* nextAttackData = pPlayer_->GetComboAttackController()->GetAttackByName(nextAttackType);

                    if (nextAttackData) {
                        BaseComboAattackBehavior::ChangeNextCombo(
                            std::make_unique<DynamicComboAttack>(pPlayer_, nextAttackData));
                    } else {
                        // データが見つからない場合はルートに戻る
                        pPlayer_->ChangeComboBehavior(std::make_unique<ComboAttackRoot>(pPlayer_));
                    }
                } else {
                    // 次の攻撃が設定されていない場合はルートに戻る
                    pPlayer_->ChangeComboBehavior(std::make_unique<ComboAttackRoot>(pPlayer_));
                }
            }
        }
    }
}

void DynamicComboAttack::ApplyMovement() {
    moveEasing_.Update(atkSpeed_);
    pPlayer_->SetWorldPosition(currentMoveValue_);
}

void DynamicComboAttack::SetupCollision() {
    auto& attackParam    = attackData_->GetAttackParam();
    auto& collisionParam = attackParam.collisionPara;

    // コリジョンサイズ設定
    pPlayer_->GetPlayerCollisionInfo()->SetSize(collisionParam.size);

    // コリジョンを有効化
    pPlayer_->GetPlayerCollisionInfo()->SetIsCollision(true);

    collisionTimer_    = 0.0f;
    isCollisionActive_ = true;
}

void DynamicComboAttack::Debug() {
#ifdef _DEBUG

#endif
}