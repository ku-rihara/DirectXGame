#include "DynamicComboAttack.h"
#include "CollisionBox/PlayerCollisionInfo.h"
#include "ComboAttackRoot.h"
#include "Frame/Frame.h"
#include "Player/ComboCreator/PlayerComboAttackController.h"
#include "Player/Player.h"

DynamicComboAttack::DynamicComboAttack(Player* player, PlayerComboAttackData* attackData)
    : BaseComboAattackBehavior(attackData->GetGroupName(), player) {

    // attackDataセット
    attackData_     = attackData;
    pCollisionInfo_ = pPlayer_->GetPlayerCollisionInfo();
    // 初期化
    Init();
}

DynamicComboAttack::~DynamicComboAttack() {}

void DynamicComboAttack::Init() {

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

    SetMoveEasing();

    //// サウンド再生
    // pPlayer_->SoundPunch();

    order_ = Order::INIT;
}

void DynamicComboAttack::Update() {

    // 通常移動
    if (attackData_->GetAttackParam().moveParam.isAbleInputMoving) {
        pPlayer_->Move(pPlayerParameter_->GetParamaters().moveSpeed);
    }

    currentFrame_ += atkSpeed_;

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

    case Order::CHANGE:
        ChangeNextAttack();
        break;
    }
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

    pCollisionInfo_->TimerUpdate(atkSpeed_);
    pCollisionInfo_->Update();

    // 演出更新
    if (attackRendition_) {
        attackRendition_->Update(atkSpeed_);

        attackRendition_->Update(atkSpeed_);
    }

    // イージングが完了したらRecoveryへ
    if (moveEasing_.IsFinished() && pCollisionInfo_->GetIsFinish()) {
        order_ = Order::WAIT;
    }
}

void DynamicComboAttack::UpdateWait() {

    AttackCancel();

    pPlayer_->AdaptRotate();
    waitTime_ += atkSpeed_;

    if (!attackData_->IsWaitFinish(waitTime_)) {
        return;
    }

    order_ = Order::CHANGE;
}

void DynamicComboAttack::ChangeNextAttack() {

    //  自動進行フラグがtrueの場合も次に進む
    bool shouldAdvance = isReserveNextCombo_ || isAttackCancel_ || attackData_->GetAttackParam().timingParam.isAutoAdvance;

    // 次のコンボに移動する
    if (nextAttackData_ && shouldAdvance) {

        BaseComboAattackBehavior::ChangeNextCombo(
            std::make_unique<DynamicComboAttack>(pPlayer_, nextAttackData_));

        return;

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

    isReserveNextCombo_ = attackData_->IsReserveNextAttack(currentFrame_, nextAttackData_->GetAttackParam().triggerParam);
}

void DynamicComboAttack::AttackCancel() {

    // タイミング
    const float cancelStartFrame = attackData_->GetAttackParam().timingParam.cancelTime;

    // 先行入力受付
    if (currentFrame_ <= cancelStartFrame) {
        return;
    }

    isAttackCancel_ = attackData_->IsCancelAttack(currentFrame_, nextAttackData_->GetAttackParam().triggerParam);

    if (isAttackCancel_) {
        // 次の攻撃
        order_ = Order::CHANGE;
    }
}

void DynamicComboAttack::ApplyMovement() {
    moveEasing_.Update(atkSpeed_);
    pPlayer_->SetWorldPosition(currentMoveValue_);
}

void DynamicComboAttack::SetupCollision() {
    auto& attackParam    = attackData_->GetAttackParam();
    auto& collisionParam = attackParam.collisionParam;

    // あらかじめコリジョンボックスを更新
    pCollisionInfo_->Update();

    // コリジョンサイズ設定
    pPlayer_->GetPlayerCollisionInfo()->SetSize(collisionParam.size);

    // コリジョンを有効化
    pPlayer_->GetPlayerCollisionInfo()->SetIsAbleCollision(true);

    // 攻撃スピード
    const PlayerComboAttackController* attackController = pPlayer_->GetComboAttackController();
    atkSpeed_                                           = attackController->GetRealAttackSpeed(Frame::DeltaTimeRate());

    // 攻撃力
    float power = attackData_->GetAttackParam().power * attackController->GetPowerRate();
    pCollisionInfo_->SetAttackPower(power);

    // アタックスタート
    pCollisionInfo_->AttackStart(attackData_);

    collisionTimer_    = 0.0f;
    isCollisionActive_ = true;
}

void DynamicComboAttack::SetMoveEasing() {
    // targetPosを計算
    const PlayerComboAttackData::MoveParam& moveParam = attackData_->GetAttackParam().moveParam;
    startPosition_                                    = pPlayer_->GetWorldPosition();

    // isPositionSelectフラグに基づいて目標位置を設定
    if (moveParam.isPositionYSelect) {
        // Y軸のみ絶対位置を使用、XとZはオフセット計算
        Vector3 offsetMove = pPlayer_->GetTransform().CalcForwardOffset(moveParam.value);
        targetPosition_.x  = startPosition_.x + offsetMove.x;
        targetPosition_.y  = offsetMove.y;
        targetPosition_.z  = startPosition_.z + offsetMove.z;
    } else {
        // 従来通りの相対的なオフセット計算
        targetPosition_ = startPosition_ + pPlayer_->GetTransform().CalcForwardOffset(moveParam.value);
    }

    // イージングのセットアップ
    moveEasing_.SetType(static_cast<EasingType>(moveParam.easeType));
    moveEasing_.SetStartValue(startPosition_);
    moveEasing_.SetEndValue(targetPosition_);
    moveEasing_.SetAdaptValue(&currentMoveValue_);
}

void DynamicComboAttack::Debug() {
#ifdef _DEBUG

#endif
}