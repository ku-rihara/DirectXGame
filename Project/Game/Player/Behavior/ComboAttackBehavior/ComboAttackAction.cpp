#include "ComboAttackAction.h"
// Behavior
#include "ComboAttackRoot.h"
// Frame
#include "Frame/Frame.h"
// Input
#include "input/Input.h"
// Player
#include "Player/CollisionBox/PlayerAttackCollisionBox.h"
#include "Player/ComboCreator/PlayerComboAttackController.h"
#include "Player/Player.h"
// ActionBehavior
#include "Player/Behavior/PlayerBehavior/PlayerJump.h"
#include "Player/Behavior/PlayerBehavior/PlayerMove.h"
// PostEffect
#include "PostEffect/PostEffectRenderer.h"

ComboAttackAction::ComboAttackAction(Player* player, PlayerComboAttackData* attackData)
    : BaseComboAttackBehavior(attackData->GetGroupName(), player) {

    // attackDataセット
    attackData_     = attackData;
    pCollisionInfo_ = pOwner_->GetPlayerCollisionInfo();
    // 初期化
    Init();
}

ComboAttackAction::~ComboAttackAction() {}

void ComboAttackAction::Init() {

    // タイミングのリセット
    currentFrame_      = 0.0f;
    waitTime_          = 0.0f;
    collisionTimer_    = 0.0f;
    isCollisionActive_ = false;
    hasHitEnemy_       = false;
    isReserveNextCombo_ = false;
    isAttackCancel_ = false;
    selectedBranchIndex_ = -1;

    // 次の攻撃候補リストを構築
    nextAttackCandidates_.clear();
    const auto& branches = attackData_->GetAttackParam().comboBranches;
    PlayerComboAttackController* controller = pOwner_->GetComboAttackController();

    for (const auto& branch : branches) {
        if (!branch.nextAttackName.empty() && branch.nextAttackName != "None") {
            PlayerComboAttackData* nextAttack = controller->GetAttackByName(branch.nextAttackName);
            if (nextAttack) {
                NextAttackCandidate candidate;
                candidate.branch = &branch;
                candidate.attackData = nextAttack;
                nextAttackCandidates_.push_back(candidate);
            }
        }
    }

    attackRendition_ = std::make_unique<PlayerAttackRendition>();
    attackRendition_->Init(pOwner_, attackData_);

    SetMoveEasing();

    order_ = Order::INIT;
}

void ComboAttackAction::Update(float atkSpeed) {

    // 通常移動
    if (attackData_->GetAttackParam().moveParam.isAbleInputMoving) {
        pOwner_->Move(pPlayerParameter_->GetParameters().moveSpeed);
    }

    currentFrame_ += atkSpeed;

    switch (order_) {

        // 攻撃初期化
    case Order::INIT:
        InitializeAttack();
        break;

        // 攻撃更新
    case Order::ATTACK:
        UpdateAttack(atkSpeed);
        break;

    case Order::WAIT:
        UpdateWait(atkSpeed);
        break;

    case Order::CHANGE:
        ChangeNextAttack();
        break;
    }
}

void ComboAttackAction::InitializeAttack() {

    // コリジョン開始時間をチェック
    float collisionStartTime = attackData_->GetAttackParam().collisionParam.startTime;

    // 開始時間が0より大きい場合は遅延させる
    if (currentFrame_ >= collisionStartTime) {
        SetupCollision();
        order_ = Order::ATTACK;
    }
}

void ComboAttackAction::UpdateAttack(float atkSpeed) {
    // 移動適用
    ApplyMovement(atkSpeed);

    // 予約入力
    PreOderNextComboForButton();

    // キャンセル処理
    AttackCancel();

    // コリジョン開始時間のチェック
    float collisionStartTime = attackData_->GetAttackParam().collisionParam.startTime;

    // まだコリジョンが有効化されていない場合
    if (!isCollisionActive_ && currentFrame_ >= collisionStartTime) {
        SetupCollision();
    }

    // コリジョン判定
    if (isCollisionActive_) {
        pCollisionInfo_->TimerUpdate(atkSpeed);
        pCollisionInfo_->Update();
    }

    // 敵にヒットしたかをチェック
    if (pCollisionInfo_->GetHasHitEnemy()) {
        hasHitEnemy_ = true;
    }

    // 演出更新
    if (attackRendition_) {
        attackRendition_->Update(atkSpeed);
    }

    // イージングが完了したらWaitへ
    if (moveEasing_.IsFinished() && pCollisionInfo_->GetIsFinish()) {
        order_ = Order::WAIT;
    }
}

void ComboAttackAction::UpdateWait(float atkSpeed) {

    AttackCancel();

    pOwner_->AdaptRotate();
    waitTime_ += atkSpeed;

    if (!attackData_->IsWaitFinish(waitTime_)) {
        return;
    }

    order_ = Order::CHANGE;
}

void ComboAttackAction::ChangeNextAttack() {

    // 自動進行フラグがtrueの場合も次に進む（最初の分岐を選択）
    bool shouldAdvance = isReserveNextCombo_ || isAttackCancel_ || attackData_->GetAttackParam().timingParam.isAutoAdvance;
    KetaEngine::Input::SetVibration(0, 0.0f, 0.0f);

    // 次のコンボに移動する
    if (shouldAdvance && selectedBranchIndex_ >= 0 && selectedBranchIndex_ < static_cast<int32_t>(nextAttackCandidates_.size())) {
        PlayerComboAttackData* nextAttackData = nextAttackCandidates_[selectedBranchIndex_].attackData;

        BaseComboAttackBehavior::ChangeNextCombo(
            std::make_unique<ComboAttackAction>(pOwner_, nextAttackData));

        return;

    } else if (shouldAdvance && attackData_->GetAttackParam().timingParam.isAutoAdvance && !nextAttackCandidates_.empty()) {
        // 自動進行の場合は最初の分岐を使用
        PlayerComboAttackData* nextAttackData = nextAttackCandidates_[0].attackData;

        BaseComboAttackBehavior::ChangeNextCombo(
            std::make_unique<ComboAttackAction>(pOwner_, nextAttackData));

        return;

    } else {
        // 落下フラグがある場合はPlayerJumpに移行
        if (attackData_->GetAttackParam().fallParam.enableFall) {
            pOwner_->ChangeBehavior(std::make_unique<PlayerJump>(pOwner_, true));
            pOwner_->ChangeComboBehavior(std::make_unique<ComboAttackRoot>(pOwner_));
            return;
        }

        // データが見つからない場合はルートに戻る
        KetaEngine::PostEffectRenderer::GetInstance()->SetPostEffectMode(KetaEngine::PostEffectMode::NONE);
        pOwner_->ChangeBehavior(std::make_unique<PlayerMove>(pOwner_));
        pOwner_->ChangeComboBehavior(std::make_unique<ComboAttackRoot>(pOwner_));
    }
}

///  コンボ移動フラグ処理
void ComboAttackAction::PreOderNextComboForButton() {

    if (nextAttackCandidates_.empty()) {
        isReserveNextCombo_ = false;
        return;
    }

    // 全ての分岐をチェック
    for (size_t i = 0; i < nextAttackCandidates_.size(); ++i) {
        const auto& candidate = nextAttackCandidates_[i];

        // ヒット状態を渡して先行入力をチェック
        if (attackData_->IsReserveNextAttack(currentFrame_, *candidate.branch, hasHitEnemy_)) {
            isReserveNextCombo_ = true;
            selectedBranchIndex_ = static_cast<int32_t>(i);
            return;
        }
    }

    isReserveNextCombo_ = false;
}

void ComboAttackAction::AttackCancel() {

    if (nextAttackCandidates_.empty()) {
        return;
    }

    // 全ての分岐をチェック
    for (size_t i = 0; i < nextAttackCandidates_.size(); ++i) {
        const auto& candidate = nextAttackCandidates_[i];

        // キャンセル時間をチェック
        if (attackData_->IsCancelAttack(currentFrame_, *candidate.branch, hasHitEnemy_)) {
            isAttackCancel_ = true;
            selectedBranchIndex_ = static_cast<int32_t>(i);
            order_ = Order::CHANGE;
            return;
        }
    }
}

void ComboAttackAction::ApplyMovement(float atkSpeed) {
    moveEasing_.Update(atkSpeed);
    pOwner_->SetWorldPosition(currentMoveValue_);
}

void ComboAttackAction::SetupCollision() {
    auto& attackParam    = attackData_->GetAttackParam();
    auto& collisionParam = attackParam.collisionParam;

    // あらかじめコリジョンボックスを更新
    pCollisionInfo_->Update();

    // コリジョンサイズ設定
    pOwner_->GetPlayerCollisionInfo()->SetSize(collisionParam.size);

    // コリジョンを有効化
    pOwner_->GetPlayerCollisionInfo()->SetIsAbleCollision(true);

    // 攻撃スピード
    const PlayerComboAttackController* attackController = pOwner_->GetComboAttackController();

    // 攻撃力
    float power = attackData_->GetAttackParam().power * attackController->GetPowerRate();
    pCollisionInfo_->SetAttackPower(power);

    // アタックスタート
    pCollisionInfo_->AttackStart(attackData_);

    collisionTimer_    = 0.0f;
    isCollisionActive_ = true;
}

void ComboAttackAction::SetMoveEasing() {
    // targetPosを計算
    const PlayerComboAttackData::MoveParam& moveParam = attackData_->GetAttackParam().moveParam;
    startPosition_                                    = pOwner_->GetWorldPosition();

    // オフセット計算
    targetPosition_ = startPosition_ + pOwner_->GetBaseTransform().CalcForwardOffset(moveParam.value);

    // Yだけ指定する場合
    if (moveParam.isPositionYSelect) {
        targetPosition_.y = moveParam.value.y;
    }

    // イージングのセットアップ
    moveEasing_.SetType(static_cast<EasingType>(moveParam.easeType));
    moveEasing_.SetStartValue(startPosition_);
    moveEasing_.SetEndValue(targetPosition_);
    moveEasing_.SetMaxTime(moveParam.easeTime);
    moveEasing_.SetAdaptValue(&currentMoveValue_);
    moveEasing_.SetFinishTimeOffset(moveParam.finishTimeOffset);

    // 開始時間を設定
    moveEasing_.SetStartTimeOffset(moveParam.startTime);
}

void ComboAttackAction::Debug() {
#ifdef _DEBUG

#endif
}