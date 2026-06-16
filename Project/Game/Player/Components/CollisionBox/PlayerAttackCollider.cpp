#include "PlayerAttackCollider.h"
// ComboData
#include "Player/ComboCreator/PlayerComboAttackData.h"
// enemy
#include "Enemy/Types/BaseEnemy.h"
// Frame
#include "Frame/Frame.h"
// imGui

void PlayerAttackCollider::Init() {

    // フラグの初期化
    SetIsAbleCollision(false);
    isInLoopWait_ = false;

    // カウント、タイマーの初期化
    currentLoopCount_ = 0;
    loopWaitTimer_    = 0.0f;

    // transformの初期化
    transform_.Init();
}

void PlayerAttackCollider::Update() {

    // 攻撃データが無くてもColliderの更新は行う
    if (!comboAttackData_) {
        AdaptCollision();
        return;
    }

    // プレイヤーを常に追従
    if (comboAttackData_->GetAttackParamForPhase(phase_).collisionParam.isAlwaysFollowing) {
        UpdateOffset();
    }

    // Collider更新
    AdaptCollision();
}

void PlayerAttackCollider::AdaptCollision() {
    SetCollisionRadius(sphereRad_);
    SetIsAdaptCollision(isAbleCollision_);
    transform_.UpdateMatrix();
}

void PlayerAttackCollider::TimerUpdate(float timeSpeed) {
    if (!comboAttackData_) {
        return;
    }

    // コリジョンパラメータ取得
    const auto& collisionParam = comboAttackData_->GetAttackParamForPhase(phase_).collisionParam;

    // ループ待機処理
    LoopWaiting(timeSpeed);

    // 通常のコリジョンタイマー処理
    if (!isAbleCollision_) {
        return;
    }

    // コリジョン適用タイマー減算
    adaptTimer_ -= timeSpeed;

    if (adaptTimer_ > 0.0f) {
        return;
    }

    // 1回の攻撃が終了
    SetIsAbleCollision(false);

    // ループが設定されている場合
    if (collisionParam.loopNum > 0 && currentLoopCount_ < collisionParam.loopNum) {
        // ループ待機状態に入る
        isInLoopWait_  = true;
        loopWaitTimer_ = collisionParam.loopWaitTime;
    } else {
        // 攻撃の終了
        isInLoopWait_ = false;
        isFinish_     = true;
        SetIsAbleCollision(false);
    }
}

void PlayerAttackCollider::LoopWaiting(float timeSpeed) {
    // ループ待機中の処理
    if (!isInLoopWait_) {
        return;
    }

    // ループ待機タイム減算
    loopWaitTimer_ -= timeSpeed;

    // ループタイムが0になるまで減算
    if (loopWaitTimer_ > 0.0f) {
        return;
    }

    // 待機終了、次のループ開始
    currentLoopCount_++;

    // まだループ回数が残っている場合
    LoopStart();
}

void PlayerAttackCollider::LoopStart() {
    // コリジョンパラメータ取得
    const auto& collisionParam = comboAttackData_->GetAttackParamForPhase(phase_).collisionParam;

    isInLoopWait_ = false;
    adaptTimer_   = collisionParam.adaptTime;
    // ループ開始時にヒットフラグをリセット
    isHit_ = false;
    SetIsAbleCollision(true);
    UpdateOffset();
}

void PlayerAttackCollider::AttackStart(const PlayerComboAttackData* comboAttackData, AttackTimelinePhase phase) {
    comboAttackData_ = comboAttackData;
    phase_           = phase;

    // collision情報を取得
    const auto& collisionParam = comboAttackData_->GetAttackParamForPhase(phase_).collisionParam;

    // 初期パラメータセット
    adaptTimer_       = collisionParam.adaptTime;
    currentLoopCount_ = 0;
    loopWaitTimer_    = 0.0f;
    isInLoopWait_     = false;
    isFinish_         = false;
    isHit_            = false;
    damageHitCount_   = 0;
    hasHitTarget_     = false;
    hitTargetPos_     = {};

    // サイズセット
    sphereRad_ = collisionParam.sphereRad;

    // collision位置Offset
    UpdateOffset();

    // コリジョン可能に
    SetIsAbleCollision(true);
}

void PlayerAttackCollider::UpdateOffset() {
    Vector3 offSetValue = comboAttackData_->GetAttackParamForPhase(phase_).collisionParam.offsetPos;
    offset_             = offSetValue;
}

void PlayerAttackCollider::OnCollisionStay([[maybe_unused]] BaseCollider* other) {
    if (BaseEnemy* enemy = dynamic_cast<BaseEnemy*>(other)) {
        isHit_ = true;
        // 最初にヒットした敵の座標を記録し、毎フレーム更新する
        if (!hasHitTarget_) {
            hasHitTarget_ = true;
        }
        hitTargetPos_ = enemy->GetCollisionPos();
    }
}

void PlayerAttackCollider::PrepareForNewAttack() {
    isFinish_       = false;
    isHit_          = false;
    hasHitTarget_   = false;
    damageHitCount_ = 0;
    SetIsAbleCollision(false);
}

void PlayerAttackCollider::SetPlayerBaseTransform(const KetaEngine::WorldTransform* playerBaseTransform) {
    transform_.parent_ = playerBaseTransform;
}

Vector3 PlayerAttackCollider::GetCollisionPos() const {
    // ワールド座標に変換
    Vector3 worldPos = TransformMatrix(offset_, transform_.matWorld_);
    return worldPos;
}
