#include "DynamicComboAttack.h"
#include "ComboAttackRoot.h"
#include "Player/ComboCreator/PlayerComboAttackController.h"
#include "Player/Player.h"

DynamicComboAttack::DynamicComboAttack(Player* player, PlayerComboAttackData* attackData)
    : BaseComboAattackBehavior(attackData->GetGroupName(), player) {

    // attackDataセット
    attackData_ = attackData;
    // 初期化
    Init();
}

DynamicComboAttack::~DynamicComboAttack() {}

void DynamicComboAttack::Init() {
    BaseComboAattackBehavior::Init();

    // タイミングのリセット
    currentFrame_      = 0.0f;
    waitTime_          = 0.0f;
    isCollisionActive_ = false;
    collisionTimer_    = 0.0f;

    // 次の攻撃データを取得
    nextAttackName_ = attackData_->GetAttackParam().nextAttackType;
    if (nextAttackName_ != "None" && !nextAttackName_.empty()) {
        nextAttackData_ = pPlayer_->GetComboAttackController()->GetAttackByName(nextAttackName_);
    }

    attackRendition_ = std::make_unique<PlayerAttackRendition>();
    attackRendition_->Init(pPlayer_, attackData_);

    // targetPosを計算
    const PlayerComboAttackData::MoveParam& moveParam = attackData_->GetAttackParam().moveParam;
    startPosition_                                    = pPlayer_->GetWorldPosition();
    targetPosition_                                   = pPlayer_->GetTransform().CalcForwardTargetPos(startPosition_, moveParam.value);

    // イージングのセットアップ
    moveEasing_.SetType(static_cast<EasingType>(moveParam.easeType));
    moveEasing_.SetStartValue(startPosition_);
    moveEasing_.SetEndValue(targetPosition_);
    moveEasing_.SetAdaptValue(&currentMoveValue_);

    moveEasing_.SetOnFinishCallback([this]() {
        order_ = Order::WAIT;
    });

    //// サウンド再生
    // pPlayer_->SoundPunch();

    order_ = Order::INIT;
}

void DynamicComboAttack::Update() {

    // 通常移動
    pPlayer_->Move(pPlayerParameter_->GetParamaters().moveSpeed);

    switch (order_) {

        // 攻撃初期化
    case Order::INIT:
        InitializeAttack();
        break;

        // 攻撃更新
    case Order::ATTACK:
        UpdateAttack();
        break;

    case Order::WAIT:
        UpdateWait();
        break;
    }

    currentFrame_ += atkSpeed_;
}

void DynamicComboAttack::InitializeAttack() {

    SetupCollision();
    order_ = Order::ATTACK;
}

void DynamicComboAttack::UpdateAttack() {
    // 移動適用
    ApplyMovement();

    // 予約入力
    PreOderNextComboForButton();

    // キャンセル処理
    AttackCancel();

    // コリジョン判定
    auto& collisionParam = attackData_->GetAttackParam().collisionParam;

    collisionTimer_ += atkSpeed_;
    attackRendition_->Update(atkSpeed_);

    if (collisionTimer_ >= collisionParam.adaptTime) {
        // コリジョンを無効化
        isCollisionActive_ = false;
        pPlayer_->GetPlayerCollisionInfo()->SetIsCollision(false);
    } else {
        isCollisionActive_ = true;
        pPlayer_->GetPlayerCollisionInfo()->SetIsCollision(isCollisionActive_);

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
        order_ = Order::WAIT;
    }
}

void DynamicComboAttack::UpdateWait() {

    pPlayer_->AdaptRotate();
    waitTime_ += atkSpeed_;

    if (!attackData_->IsWaitFinish(waitTime_)) {
        return;
    }

    // 次の攻撃
    ChangeNextAttack();
}

void DynamicComboAttack::ChangeNextAttack() {
   
    // 次のコンボに移動する
    if (nextAttackData_) {

        BaseComboAattackBehavior::ChangeNextCombo(
            std::make_unique<DynamicComboAttack>(pPlayer_, nextAttackData_));

    } else {
        // データが見つからない場合はルートに戻る
        pPlayer_->ChangeComboBehavior(std::make_unique<ComboAttackRoot>(pPlayer_));
    }
}

///  コンボ移動フラグ処理
void DynamicComboAttack::PreOderNextComboForButton() {

    if (!nextAttackData_) {
        isReserveNextCombo_ = false;
        return;
    }

    isReserveNextCombo_ = attackData_->IsReserveNextAttack(currentFrame_,nextAttackData_->GetAttackParam().triggerParam);
}

void DynamicComboAttack::AttackCancel() {

    // タイミング
    const float cancelStartFrame = attackData_->GetAttackParam().timingParam.cancelTime;
   
    // 先行入力受付
    if (currentFrame_ <= cancelStartFrame) {
        return;
    }

    isAbleCancel_ = attackData_->IsAbleCancel(currentFrame_, nextAttackData_->GetAttackParam().triggerParam);

    if (isAbleCancel_) {
        // 次の攻撃
        ChangeNextAttack();
    }
}

void DynamicComboAttack::ApplyMovement() {
    moveEasing_.Update(atkSpeed_);
    pPlayer_->SetWorldPosition(currentMoveValue_);
}

void DynamicComboAttack::SetupCollision() {
    auto& attackParam    = attackData_->GetAttackParam();
    auto& collisionParam = attackParam.collisionParam;

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