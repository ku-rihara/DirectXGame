#include "PlayerAttackCollider.h"
// ComboData
#include "Player/ComboCreator/PlayerComboAttackData.h"
// enemy
#include "Enemy/Types/BaseEnemy.h"
// Frame
#include "Frame/Frame.h"
// imGui
#include <imgui.h>

void PlayerAttackCollider::Init() {
    SetIsAbleCollision(false);
    currentLoopCount_ = 0;
    isInLoopWait_     = false;
    loopWaitTimer_    = 0.0f;
    transform_.Init();
}

void PlayerAttackCollider::Update() {
    if (!comboAttackData_) {
        AdaptCollision();
        return;
    }

    // プレイヤーを常に追従
    if (comboAttackData_->GetAttackParam().collisionParam.isAlwaysFollowing) {
        UpdateOffset();
    }

    // baseの更新
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

    const auto& collisionParam = comboAttackData_->GetAttackParam().collisionParam;

    LoopWaiting(timeSpeed);

    // 通常のコリジョンタイマー処理
    if (!isAbleCollision_) {
        return;
    }

    adaptTimer_ -= timeSpeed;

    if (adaptTimer_ > 0.0f) {
        return;
    }

    SetIsAbleCollision(false);

    // ループが設定されている場合
    if (collisionParam.loopNum > 0 && currentLoopCount_ < collisionParam.loopNum) {
        // ループ待機状態に入る
        isInLoopWait_  = true;
        loopWaitTimer_ = collisionParam.loopWaitTime;
    } else {

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
    const auto& collisionParam = comboAttackData_->GetAttackParam().collisionParam;

    isInLoopWait_ = false;
    adaptTimer_   = collisionParam.adaptTime;
    // ループ開始時にヒットフラグをリセット（マルチヒット用）
    isHit_ = false;
    SetIsAbleCollision(true);
    UpdateOffset();
}

void PlayerAttackCollider::AttackStart(const PlayerComboAttackData* comboAttackData) {
    comboAttackData_ = comboAttackData;

    // collision情報を取得
    const auto& collisionParam = comboAttackData_->GetAttackParam().collisionParam;

    // 初期パラメータセット
    adaptTimer_       = collisionParam.adaptTime;
    currentLoopCount_ = 0;
    loopWaitTimer_    = 0.0f;
    isInLoopWait_     = false;
    isFinish_        = false;
    isHit_           = false;
    hasHitEnemy_     = false;
    damageHitCount_  = 0;
    hasHitTarget_  = false;
    hitTargetPos_  = {};

    // サイズセット
    sphereRad_ = collisionParam.sphereRad;

    // collision位置Offset
    UpdateOffset();

    // コリジョン可能に
    SetIsAbleCollision(true);
}

void PlayerAttackCollider::UpdateOffset() {
    Vector3 offSetValue = comboAttackData_->GetAttackParam().collisionParam.offsetPos;
    offset_             = offSetValue;
}

void PlayerAttackCollider::OnCollisionStay([[maybe_unused]] BaseCollider* other) {
    if (BaseEnemy* enemy = dynamic_cast<BaseEnemy*>(other)) {
        isHit_        = true;
        hasHitEnemy_  = true;
        // 最初にヒットした敵の座標を記録し、毎フレーム更新する
        if (!hasHitTarget_) {
            hasHitTarget_ = true;
        }
        hitTargetPos_ = enemy->GetCollisionPos();
    }
}

void PlayerAttackCollider::SetPlayerBaseTransform(const KetaEngine::WorldTransform* playerBaseTransform) {
    baseTransform_     = playerBaseTransform;
    transform_.parent_ = playerBaseTransform;
}

Vector3 PlayerAttackCollider::GetCollisionPos() const {
    // ワールド座標に変換
    Vector3 worldPos = TransformMatrix(offset_, transform_.matWorld_);
    return worldPos;
}

