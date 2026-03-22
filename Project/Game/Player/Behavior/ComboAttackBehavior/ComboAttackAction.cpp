#include "ComboAttackAction.h"
#include <cmath>
// Behavior
#include "ComboAttackRoot.h"
// Frame
#include "Frame/Frame.h"
// Input
#include "input/Input.h"
// Player
#include "Player/Components/CollisionBox/PlayerAttackCollider.h"
#include "Player/ComboCreator/PlayerComboAttackController.h"
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
    auto& branches                          = attackData_->GetComboBranches();
    PlayerComboAttackController* controller = pOwner_->GetComboAttackController();

    for (auto& branch : branches) {
        if (!branch->GetNextAttackName().empty() && branch->GetNextAttackName() != "None") {
            PlayerComboAttackData* nextAttack = controller->GetAttackByName(branch->GetNextAttackName());

            if (IsAttackUnlock(*nextAttack)) {
                NextAttackCandidate candidate;
                candidate.branch     = branch.get();
                candidate.attackData = nextAttack;
                nextAttackCandidates_.push_back(candidate);
            }
        }
    }

    attackRendition_ = std::make_unique<PlayerAttackRendition>();
    attackRendition_->Init(pOwner_, attackData_);

    SetMoveEasing();

    order_ = Order::INIT;

    // 攻撃開始を通知
    pOwner_->FireAutoComboAttackCallback(attackData_->GetGroupName());
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

    order_ = Order::ATTACK;
}

void ComboAttackAction::UpdateAttack(float atkSpeed) {
    // 移動適用
    ApplyMovement(atkSpeed);

    // 予約入力
    PreOderNextComboForButton();
    TryAutoSelectNextFromQueue();

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

    // ヒットした敵の方向を向き続ける
    if (pCollisionInfo_->GetHasHitTarget()) {
        Vector3 dir = pCollisionInfo_->GetHitTargetPos() - pOwner_->GetWorldPosition();
        if (dir.x != 0.0f || dir.z != 0.0f) {
            pOwner_->SetObjectiveAngle(std::atan2(dir.x, dir.z));
        }
        pOwner_->AdaptRotate();
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

    order_ = Order::CHANGE;
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
        if (pOwner_->GetWorldPosition().y >= pOwner_->GetParameter()->GetParameters().startPos_.y) {
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
            order_               = Order::CHANGE;
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
    pOwner_->GetPlayerCollisionInfo()->SetSphereRad(collisionParam.sphereRad);

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

bool ComboAttackAction::IsAttackUnlock(const PlayerComboAttackData& data) const {
    bool result = data.GetAttackParam().isUnlocked || pOwner_->GetIsIgnoreUnlockState();
    return result;
}

Vector3 ComboAttackAction::CalcStopBeforeEnemyTarget(
    const Vector3& start, const Vector3& defaultTarget,
    Vector3* outFoundEnemyPos) const {

    EnemyManager* enemyManager = attackData_->GetEnemyManager();
    if (!enemyManager) {
        return defaultTarget;
    }

    const auto& enemies = enemyManager->GetEnemies();
    if (enemies.empty()) {
        return defaultTarget;
    }

    // 移動方向ベクトル
    Vector3 moveVec  = defaultTarget - start;
    float   moveDist = moveVec.Length();
    if (moveDist < 0.001f) {
        return defaultTarget;
    }
    Vector3 moveDirNorm = moveVec.Normalize();

    // 経路の横幅
    const float kPathHalfWidth = 3.0f;
    // 敵コリジョン前面から何ユニット手前で止まるか
    const float kStopMargin = 1.5f;

    float         closestDot      = moveDist; 
    bool          found           = false;
    Vector3       foundEnemyPos   = {};

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
#ifdef _DEBUG

#endif
}