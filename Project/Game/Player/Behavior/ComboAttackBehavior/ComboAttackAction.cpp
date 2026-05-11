#include "ComboAttackAction.h"
#include "utility/Log/Log.h"
#include <cmath>
// Behavior
#include "ComboAttackRoot.h"
// Frame
#include "Frame/Frame.h"
// Input
#include "input/Input.h"
// LockOn
#include "LockOn/LockOn.h"
#include "LockOn/LockOnController.h"
// Player
#include "DeathTimer/DeathTimer.h"
#include "Player/ComboCreator/PlayerComboAttackController.h"
#include "Player/Components/CollisionBox/PlayerAttackCollider.h"
#include "Player/Player.h"
// ActionBehavior
#include "Player/Behavior/PlayerBehavior/PlayerJump.h"
#include "Player/Behavior/PlayerBehavior/PlayerMove.h"
// PostEffect
#include "PostEffect/PostEffectRenderer.h"
// Enemy
#include "Enemy/EnemyManager.h"
#include "Enemy/Types/BaseEnemy.h"

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

    // 前の攻撃の残留状態をクリア
    if (pCollisionInfo_) {
        pCollisionInfo_->PrepareForNewAttack();
    }

    // タイミングのリセット
    currentFrame_            = 0.0f;
    waitTime_                = 0.0f;
    collisionTimer_          = 0.0f;
    isCollisionActive_       = false;
    hasHitEnemy_             = false;
    isReserveNextCombo_      = false;
    isAutoReservedCombo_     = false;
    autoSelectedBranchIndex_ = -1;
    isAttackCancel_          = false;
    selectedBranchIndex_     = -1;

    // 次の攻撃候補リストを構築
    nextAttackCandidates_.clear();
    allNextAttackCandidates_.clear();
    auto& branches                          = attackData_->GetComboBranches();
    PlayerComboAttackController* controller = pOwner_->GetComboAttackController();

    for (auto& branch : branches) {
        if (!branch->GetNextAttackName().empty() && branch->GetNextAttackName() != "None") {
            PlayerComboAttackData* nextAttack = controller->GetAttackByName(branch->GetNextAttackName());
            if (!nextAttack) {
                continue;
            }

            NextAttackCandidate candidate;
            candidate.branch     = branch.get();
            candidate.attackData = nextAttack;

            // 全候補（ロック中を含む）はHintUI用
            allNextAttackCandidates_.push_back(candidate);

            // ロック解除済みのみ入力受付用に追加
            if (IsAttackUnlock(*nextAttack)) {
                nextAttackCandidates_.push_back(candidate);
            }
        }
    }

    attackRendition_ = std::make_unique<PlayerAttackRendition>();
    attackRendition_->Init(pOwner_, attackData_, AttackTimelinePhase::MAIN);

    prepRendition_.reset();
    finishRendition_.reset();

    if (attackData_->HasPrepPhase()) {
        prepRendition_ = std::make_unique<PlayerAttackRendition>();
        prepRendition_->Init(pOwner_, attackData_, AttackTimelinePhase::PREPARATION);
    }
    if (attackData_->HasFinishPhase()) {
        finishRendition_ = std::make_unique<PlayerAttackRendition>();
        finishRendition_->Init(pOwner_, attackData_, AttackTimelinePhase::FINISH);
    }

    // アニメーションを止めてから移動イージングを設定する
    pOwner_->GetPlayerAnimator().StopMoveAnimation();

    if (attackData_->HasPrepPhase()) {
        SetPrepMoveEasing();
    } else {
        SetMoveEasing();
    }

    SetOrder(Order::INIT);

    // 攻撃開始を通知
    pOwner_->FireAutoComboAttackCallback(attackData_->GetGroupName());
}

void ComboAttackAction::Update(float atkSpeed) {

    // 死亡チェック
    if (pOwner_->GetDeathTimer() && pOwner_->GetDeathTimer()->GetIsDeath()) {
        pOwner_->ChangeCombBoRoot();
        return;
    }

    // 通常移動
    if (attackData_->GetAttackParam().moveParam.isAbleInputMoving) {
        pOwner_->Move(pPlayerParameter_->GetParameters().moveSpeed);
    }

    currentFrame_ += atkSpeed;

    if (orderFunc_) {
        orderFunc_(atkSpeed);
    }
}

void ComboAttackAction::InitializeAttack() {
    currentFrame_   = 0.0f;
    attackRawTimer_ = 0.0f;
    if (attackData_->HasPrepPhase()) {
        SetOrder(Order::PREP_ATTACK);
    } else {
        SetOrder(Order::ATTACK);
    }
}

void ComboAttackAction::UpdatePrepAttack(float atkSpeed) {
    const auto& prepParam = attackData_->GetAttackParamForPhase(AttackTimelinePhase::PREPARATION);

    prepMoveEasing_.Update(atkSpeed);
    if (!prepParam.isMotionOnly) {
        // 移動方向を向く
        if (prepParam.moveParam.isFaceMovementDirection) {
            Vector3 moveDir = prepCurrentMoveValue_ - pOwner_->GetWorldPosition();
            float lengthXZ = std::sqrt(moveDir.x * moveDir.x + moveDir.z * moveDir.z);
            if (lengthXZ > 0.001f || std::abs(moveDir.y) > 0.001f) {
                // Yaw (Y軸回転)
                if (lengthXZ > 0.001f) {
                    pOwner_->SetObjectiveAngle(std::atan2(moveDir.x, moveDir.z));
                }
                // Pitch (X軸回転) - 高低差も含めて傾く
                pOwner_->SetObjectiveAnglePitch(std::atan2(-moveDir.y, lengthXZ));
                pOwner_->AdaptRotate();
            }
        }

        pOwner_->SetWorldPosition(prepCurrentMoveValue_);
    }

    if (prepRendition_) {
        prepRendition_->Update(atkSpeed);
    }

    // コリジョン開始時間のチェック
    const float prepCollisionStartTime = prepParam.collisionParam.startTime;
    if (!isCollisionActive_ && currentFrame_ >= prepCollisionStartTime) {
        SetupCollision(AttackTimelinePhase::PREPARATION);
    }

    // コリジョン判定
    if (isCollisionActive_) {
        pCollisionInfo_->TimerUpdate(atkSpeed);
        pCollisionInfo_->Update();
    }

    // イージングとコリジョンタイムライン両方が完了したら次フェーズへ
    const float prepFinishWait = prepParam.timingParam.finishWaitTime;
    if (prepMoveEasing_.IsFinished() && pCollisionInfo_->GetIsFinish() && currentFrame_ >= prepFinishWait) {
        currentFrame_      = 0.0f;
        isCollisionActive_ = false;
        SetMoveEasing();
        SetOrder(Order::ATTACK);
    }
}

void ComboAttackAction::UpdateAttack(float atkSpeed) {
    // （HitStop等で長時間スタックした場合の脱出）
    attackRawTimer_ += KetaEngine::Frame::DeltaTime();
    if (attackRawTimer_ >= kAttackTimeout) {
        SetOrder(Order::WAIT);
        return;
    }

    // 移動適用
    ApplyMovement(atkSpeed);

    // 終了処理フェーズがある場合はFINISHで受け付けるため、MAINでは行わない
    if (!attackData_->HasFinishPhase()) {
        PreOderNextComboForButton();
        TryAutoSelectNextFromQueue();
        AttackCancel();
    }

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

    // ヒット時に終了処理へ即移行
    if (hasHitEnemy_ && attackData_->GetAttackParam().timingParam.isSkipToFinishOnHit) {
        isCollisionActive_ = false;
        if (attackData_->HasFinishPhase()) {
            currentFrame_ = 0.0f;
            SetFinishMoveEasing();
            SetOrder(Order::FINISH_ATTACK);
        } else {
            SetOrder(Order::WAIT);
        }
        return;
    }

    // 演出更新
    if (attackRendition_) {
        attackRendition_->Update(atkSpeed);
    }

    // イージングが完了したら次フェーズへ
    if (moveEasing_.IsFinished() && pCollisionInfo_->GetIsFinish()) {
        // isSkipToFinishOnHit が true かつヒットしていない場合は終了処理をスキップ
        bool skipFinish = attackData_->GetAttackParam().timingParam.isSkipToFinishOnHit && !hasHitEnemy_;
        if (attackData_->HasFinishPhase() && !skipFinish) {
            currentFrame_ = 0.0f;
            SetFinishMoveEasing();
            SetOrder(Order::FINISH_ATTACK);
        } else {
            SetOrder(Order::WAIT);
        }
    }
}

void ComboAttackAction::UpdateFinishAttack(float atkSpeed) {
    const auto& finishParam = attackData_->GetAttackParamForPhase(AttackTimelinePhase::FINISH);
    finishMoveEasing_.Update(atkSpeed);

    // 移動方向を向く
    if (finishParam.moveParam.isFaceMovementDirection) {
        Vector3 moveDir = finishCurrentMoveValue_ - pOwner_->GetWorldPosition();
        float lengthXZ = std::sqrt(moveDir.x * moveDir.x + moveDir.z * moveDir.z);
        if (lengthXZ > 0.001f || std::abs(moveDir.y) > 0.001f) {
            // Yaw
            if (lengthXZ > 0.001f) {
                pOwner_->SetObjectiveAngle(std::atan2(moveDir.x, moveDir.z));
            }
            // Pitch
            pOwner_->SetObjectiveAnglePitch(std::atan2(-moveDir.y, lengthXZ));
            pOwner_->AdaptRotate();
        }
    }

    pOwner_->SetWorldPosition(finishCurrentMoveValue_);

    if (finishRendition_) {
        finishRendition_->Update(atkSpeed);
    }

    // 終了処理フェーズでのキャンセル・先行入力受付
    PreOderNextComboForButton();
    TryAutoSelectNextFromQueue();
    AttackCancel();

    const float finishWait = attackData_->GetAttackParamForPhase(AttackTimelinePhase::FINISH).timingParam.finishWaitTime;
    if (finishMoveEasing_.IsFinished() && currentFrame_ >= finishWait) {
        SetOrder(Order::WAIT);
    }
}

void ComboAttackAction::UpdateWait(float atkSpeed) {

    AttackCancel();

    // ヒットした敵の方向を向き続ける
    if (pCollisionInfo_->GetHasHitTarget()) {
        Vector3 dir = pCollisionInfo_->GetHitTargetPos() - pOwner_->GetWorldPosition();
        if (dir.x != 0.0f || dir.z != 0.0f) {
            pOwner_->SetObjectiveAngle(std::atan2(dir.x, dir.z));
        }
    }

    pOwner_->AdaptRotate();
    waitTime_ += atkSpeed;

    if (!attackData_->IsWaitFinish(waitTime_)) {
        return;
    }

    SetOrder(Order::CHANGE);
}

void ComboAttackAction::ChangeNextAttack() {

    // 自動進行フラグがtrueの場合も次に進む
    bool shouldAdvance = isReserveNextCombo_ || isAutoReservedCombo_ || isAttackCancel_ || attackData_->GetAttackParam().timingParam.isAutoAdvance;
    KetaEngine::Input::SetVibration(0, 0.0f, 0.0f);

    // 自動予約時: 遷移直前にキューから取り出す
    auto TryDequeueIfAutoReserved = [this](PlayerComboAttackData* nextAttackData) {
        if (!isAutoReservedCombo_)
            return;
        auto& queue = pOwner_->GetAutoComboQueue();
        if (queue.Peek() == nextAttackData) {
            queue.Dequeue();
        }
    };

    // 次のコンボに移動する
    if (shouldAdvance && selectedBranchIndex_ >= 0 && selectedBranchIndex_ < static_cast<int32_t>(nextAttackCandidates_.size())) {
        PlayerComboAttackData* nextAttackData = nextAttackCandidates_[selectedBranchIndex_].attackData;

        TryDequeueIfAutoReserved(nextAttackData);
        BaseComboAttackBehavior::ChangeNextCombo(
            std::make_unique<ComboAttackAction>(pOwner_, nextAttackData));

        return;

    } else if (shouldAdvance && attackData_->GetAttackParam().timingParam.isAutoAdvance && !nextAttackCandidates_.empty()) {
        // 自動進行の場合は最初の分岐を使用
        PlayerComboAttackData* nextAttackData = nextAttackCandidates_[0].attackData;

        TryDequeueIfAutoReserved(nextAttackData);
        BaseComboAttackBehavior::ChangeNextCombo(
            std::make_unique<ComboAttackAction>(pOwner_, nextAttackData));

        return;

    } else {
        // 落下フラグがある場合はPlayerJumpに移行
        if (pOwner_->GetWorldPosition().y > pOwner_->GetParameter()->GetParameters().startPos_.y) {
            pOwner_->ChangeBehavior(std::make_unique<PlayerJump>(pOwner_));
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

    // 自動再生中はプレイヤーの手動入力を受け付けない
    if (!pOwner_->GetAutoComboQueue().IsEmpty()) {
        isReserveNextCombo_ = false;
        return;
    }

    if (nextAttackCandidates_.empty()) {
        isReserveNextCombo_ = false;
        return;
    }

    // 全ての分岐をチェック
    for (size_t i = 0; i < nextAttackCandidates_.size(); ++i) {
        const auto& candidate = nextAttackCandidates_[i];

        // ヒット状態を渡して先行入力をチェック
        if (attackData_->IsReserveNextAttack(currentFrame_, *candidate.branch, hasHitEnemy_)) {
            isReserveNextCombo_  = true;
            selectedBranchIndex_ = static_cast<int32_t>(i);
            return;
        }
    }

    isReserveNextCombo_ = false;
}

///  キューから次のコンボを自動選択
void ComboAttackAction::TryAutoSelectNextFromQueue() {
    // PreOderNextComboForButton のリセットを上書きして再アサート
    if (isAutoReservedCombo_) {
        if (!isReserveNextCombo_) {
            isReserveNextCombo_  = true;
            selectedBranchIndex_ = autoSelectedBranchIndex_;
        }
        return;
    }

    // 手動入力で既に選択済みなら何もしない
    if (isReserveNextCombo_) {
        return;
    }

    auto& queue = pOwner_->GetAutoComboQueue();
    if (queue.IsEmpty()) {
        return;
    }

    PlayerComboAttackData* nextData = queue.Peek();
    if (!nextData) {
        return;
    }

    for (size_t i = 0; i < nextAttackCandidates_.size(); ++i) {
        const auto& candidate = nextAttackCandidates_[i];

        // キューの先頭と一致するか
        if (candidate.attackData != nextData) {
            continue;
        }

        // 先行入力タイミングウィンドウが開いているか
        float precedeTime = candidate.branch->GetPrecedeInputTime();
        bool timingOk     = (currentFrame_ >= precedeTime) || attackData_->IsWaitFinish(currentFrame_);
        if (!timingOk) {
            continue;
        }

        // ヒット条件チェック
        if (candidate.branch->GetRequireHit() && !hasHitEnemy_) {
            continue;
        }

        // 自動予約確定
        isAutoReservedCombo_     = true;
        autoSelectedBranchIndex_ = static_cast<int32_t>(i);
        isReserveNextCombo_      = true;
        selectedBranchIndex_     = static_cast<int32_t>(i);
        return;
    }
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
            isAttackCancel_      = true;
            selectedBranchIndex_ = static_cast<int32_t>(i);
            SetOrder(Order::CHANGE);
            return;
        }
    }
}

void ComboAttackAction::ApplyMovement(float atkSpeed) {
    moveEasing_.Update(atkSpeed);

    // 移動方向を向く
    if (attackData_->GetAttackParam().moveParam.isFaceMovementDirection) {
        Vector3 moveDir = currentMoveValue_ - pOwner_->GetWorldPosition();
        float lengthXZ = std::sqrt(moveDir.x * moveDir.x + moveDir.z * moveDir.z);
        if (lengthXZ > 0.001f || std::abs(moveDir.y) > 0.001f) {
            // Yaw
            if (lengthXZ > 0.001f) {
                pOwner_->SetObjectiveAngle(std::atan2(moveDir.x, moveDir.z));
            }
            // Pitch
            pOwner_->SetObjectiveAnglePitch(std::atan2(-moveDir.y, lengthXZ));
            pOwner_->AdaptRotate();
        }
    }

    // 瞬間的に座標0に飛ぶバグへの対策: 移動量が極端に大きい(ワープ)場合は適用をスキップするか制限する
    // 特にFallAntipation等で初期化が間に合っていない場合を想定
    if (currentMoveValue_.Length() > 0.0001f || (currentMoveValue_ - pOwner_->GetWorldPosition()).Length() < 100.0f) {
        pOwner_->SetWorldPosition(currentMoveValue_);
    }
}

void ComboAttackAction::SetupCollision(AttackTimelinePhase phase) {
    auto& attackParam    = attackData_->GetAttackParamForPhase(phase);
    auto& collisionParam = attackParam.collisionParam;

    // あらかじめコリジョンボックスを更新
    pCollisionInfo_->Update();

    // コリジョンサイズ設定
    pOwner_->GetPlayerCollisionInfo()->SetSphereRad(collisionParam.sphereRad);

    // コリジョンを有効化
    pOwner_->GetPlayerCollisionInfo()->SetIsAbleCollision(true);

    // 攻撃スピード
    const PlayerComboAttackController* attackController = pOwner_->GetComboAttackController();

    // 攻撃力
    float power = attackParam.power * attackController->GetPowerRate();
    pCollisionInfo_->SetAttackPower(power);

    // アタックスタート
    pCollisionInfo_->AttackStart(attackData_, phase);

    collisionTimer_    = 0.0f;
    isCollisionActive_ = true;
}

void ComboAttackAction::SetMoveEasing() {
    // targetPosを計算
    const auto& attackParam = attackData_->GetAttackParam();
    const auto& moveParam   = attackParam.moveParam;
    startPosition_          = pOwner_->GetWorldPosition();

    // ロックオン対象がいればそちらを向く
    LockOn* lockOn = nullptr;

    if (pOwner_->GetLockOnController()) {
        lockOn = pOwner_->GetLockOnController()->GetLockOn();
    }

    if (lockOn && lockOn->ExistTarget()) {
        Vector3 targetPos                       = lockOn->GetCurrentTargetPosition();
        Vector3 toTarget                        = targetPos - startPosition_;
        pOwner_->GetBaseTransform().rotation_.y = std::atan2(toTarget.x, toTarget.z);
    }

    // 攻撃条件がAIRまたはBOTHなら高さを維持する。そうでなければ地上に補正する
    const float groundY = pPlayerParameter_->GetParameters().startPos_.y;
    auto condition      = attackParam.triggerParam.condition;
    bool isAirValid     = (condition == PlayerComboAttackData::TriggerCondition::AIR || 
                           condition == PlayerComboAttackData::TriggerCondition::BOTH);

    if (!isAirValid && startPosition_.y > groundY) {
        startPosition_.y = groundY;
    }

    // オフセット計算
    targetPosition_ = startPosition_ + pOwner_->GetBaseTransform().CalcForwardOffset(moveParam.value);

    // Yだけ指定する場合
    if (moveParam.isPositionYSelect) {
        targetPosition_.y = moveParam.value.y;
    } else {
        // Yを直接指定しない場合、空中攻撃なら開始時の高さを維持する
        if (isAirValid) {
            targetPosition_.y = startPosition_.y;
        }
    }

    // 移動先に敵がいたら目の前で止まる
    if (moveParam.isStopBeforeEnemy) {
        targetPosition_ = CalcStopBeforeEnemyTarget(startPosition_, targetPosition_);
    }

    // 待機中のために現在の位置で初期化
    currentMoveValue_ = startPosition_;

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

void ComboAttackAction::SetPrepMoveEasing() {
    const auto& prepParam = attackData_->GetAttackParamForPhase(AttackTimelinePhase::PREPARATION);
    const auto& moveParam = prepParam.moveParam;
    Vector3 start         = pOwner_->GetWorldPosition();

    // 攻撃条件は全フェーズ共通でMAINのものを参照する
    const float groundY = pPlayerParameter_->GetParameters().startPos_.y;
    auto condition      = attackData_->GetAttackParam().triggerParam.condition;
    bool isAirValid     = (condition == PlayerComboAttackData::TriggerCondition::AIR || 
                           condition == PlayerComboAttackData::TriggerCondition::BOTH);

    if (!isAirValid && start.y > groundY) {
        start.y = groundY;
    }

    Vector3 target = start + pOwner_->GetBaseTransform().CalcForwardOffset(moveParam.value);
    if (moveParam.isPositionYSelect) {
        target.y = moveParam.value.y;
    } else {
        // Yを直接指定しない場合、空中攻撃なら開始時の高さを維持する
        if (isAirValid) {
            target.y = start.y;
        }
    }

    prepCurrentMoveValue_ = start;

    prepMoveEasing_.SetType(static_cast<EasingType>(moveParam.easeType));
    prepMoveEasing_.SetStartValue(start);
    prepMoveEasing_.SetEndValue(target);
    prepMoveEasing_.SetMaxTime(moveParam.easeTime);
    prepMoveEasing_.SetAdaptValue(&prepCurrentMoveValue_);
    prepMoveEasing_.SetFinishTimeOffset(moveParam.finishTimeOffset);
    prepMoveEasing_.SetStartTimeOffset(moveParam.startTime);
}

void ComboAttackAction::SetFinishMoveEasing() {
    const auto& finishParam = attackData_->GetAttackParamForPhase(AttackTimelinePhase::FINISH);
    const auto& moveParam   = finishParam.moveParam;
    Vector3 start           = pOwner_->GetWorldPosition();

    // 攻撃条件は全フェーズ共通でMAINのものを参照する
    const float groundY = pPlayerParameter_->GetParameters().startPos_.y;
    auto condition      = attackData_->GetAttackParam().triggerParam.condition;
    bool isAirValid     = (condition == PlayerComboAttackData::TriggerCondition::AIR || 
                           condition == PlayerComboAttackData::TriggerCondition::BOTH);

    if (!isAirValid && start.y > groundY) {
        start.y = groundY;
    }

    Vector3 target = start + pOwner_->GetBaseTransform().CalcForwardOffset(moveParam.value);
    if (moveParam.isPositionYSelect) {
        target.y = moveParam.value.y;
    } else {
        // Yを直接指定しない場合、空中攻撃なら開始時の高さを維持する
        if (isAirValid) {
            target.y = start.y;
        }
    }

    finishCurrentMoveValue_ = start;

    finishMoveEasing_.SetType(static_cast<EasingType>(moveParam.easeType));
    finishMoveEasing_.SetStartValue(start);
    finishMoveEasing_.SetEndValue(target);
    finishMoveEasing_.SetMaxTime(moveParam.easeTime);
    finishMoveEasing_.SetAdaptValue(&finishCurrentMoveValue_);
    finishMoveEasing_.SetFinishTimeOffset(moveParam.finishTimeOffset);
    finishMoveEasing_.SetStartTimeOffset(moveParam.startTime);
}

bool ComboAttackAction::IsAttackUnlock(const PlayerComboAttackData& data) const {
    bool result = data.GetAttackParam().isUnlocked || pOwner_->GetIsIgnoreUnlockState();
    return result;
}

Vector3 ComboAttackAction::CalcStopBeforeEnemyTarget(
    const Vector3& start, const Vector3& defaultTarget,
    Vector3* outFoundEnemyPos) const {

    // ヒットした敵の方向を向き続ける
    if (pCollisionInfo_->GetHasHitTarget()) {
        Vector3 dir = pCollisionInfo_->GetHitTargetPos() - pOwner_->GetWorldPosition();
        if (dir.x != 0.0f || dir.z != 0.0f) {
            pOwner_->SetObjectiveAngle(std::atan2(dir.x, dir.z));
        }
        pOwner_->AdaptRotate();
    }

    EnemyManager* enemyManager = attackData_->GetEnemyManager();
    if (!enemyManager) {
        return defaultTarget;
    }

    const auto& enemies = enemyManager->GetEnemies();
    if (enemies.empty()) {
        return defaultTarget;
    }

    // 移動方向ベクトル
    Vector3 moveVec = defaultTarget - start;
    float moveDist  = moveVec.Length();
    if (moveDist < 0.001f) {
        return defaultTarget;
    }
    Vector3 moveDirNorm = moveVec.Normalize();

    // 経路の横幅
    const float kPathHalfWidth = 3.0f;
    // 敵コリジョン前面から何ユニット手前で止まるか
    const float kStopMargin = 1.5f;

    float closestDot      = moveDist;
    bool found            = false;
    Vector3 foundEnemyPos = {};

    for (const auto& enemy : enemies) {
        if (!enemy) {
            continue;
        }
        // 死亡・死亡待機中の敵は無視
        if (enemy->GetIsDeath() || enemy->GetIsDeathPending()) {
            continue;
        }

        Vector3 toEnemy = enemy->GetWorldPosition() - start;

        // 移動方向への射影距離
        float dot = toEnemy.Dot(moveDirNorm);

        // 移動方向の前方にいて、かつ移動距離内にいる敵のみ
        if (dot <= 0.0f || dot > moveDist) {
            continue;
        }

        // 移動経路からの横距離
        float perpDist = (toEnemy - moveDirNorm * dot).Length();

        // 敵のコリジョン幅の半分 + 経路幅を合わせた判定
        float enemyHalfWidth = enemy->GetParameter().collisionRad * 0.5f;
        if (perpDist > kPathHalfWidth + enemyHalfWidth) {
            continue;
        }

        // より手前にいる敵を優先
        if (dot < closestDot) {
            closestDot    = dot;
            found         = true;
            foundEnemyPos = enemy->GetWorldPosition();
        }
    }

    if (!found) {
        return defaultTarget;
    }

    if (outFoundEnemyPos) {
        *outFoundEnemyPos = foundEnemyPos;
    }

    // 敵の手前 kStopMargin 分の位置を目標にする
    float stopDist = closestDot - kStopMargin;
    if (stopDist <= 0.0f) {
        // 既に接触している場合はその場から動かない
        return start;
    }

    return start + moveDirNorm * stopDist;
}

void ComboAttackAction::Debug() {
#if defined(_DEBUG) || defined(DEVELOPMENT)

#endif
}

void ComboAttackAction::SetOrder(Order order) {
    order_ = order;
    switch (order) {
    case Order::INIT:
        break;
    case Order::PREP_ATTACK:
        break;
    case Order::ATTACK:
        break;
    case Order::FINISH_ATTACK:
        break;
    case Order::WAIT:
        break;
    case Order::CHANGE:
        break;
    }

    switch (order) {
    case Order::INIT:
        orderFunc_ = [this](float) {
            InitializeAttack();
        };
        break;
    case Order::PREP_ATTACK:
        orderFunc_ = [this](float speed) {
            UpdatePrepAttack(speed);
        };
        break;
    case Order::ATTACK:
        orderFunc_ = [this](float speed) {
            UpdateAttack(speed);
        };
        break;
    case Order::FINISH_ATTACK:
        orderFunc_ = [this](float speed) {
            UpdateFinishAttack(speed);
        };
        break;
    case Order::WAIT:
        orderFunc_ = [this](float speed) {
            UpdateWait(speed);
        };
        break;
    case Order::CHANGE:
        orderFunc_ = [this](float) {
            ChangeNextAttack();
        };
        break;
    }
}
