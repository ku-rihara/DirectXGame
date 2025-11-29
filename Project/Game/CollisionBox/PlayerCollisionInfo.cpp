#include "PlayerCollisionInfo.h"
#include "Frame/Frame.h"
#include "Player/ComboCreator/PlayerComboAttackData.h"
// enemy
#include "Enemy/Types/BaseEnemy.h"
#include <imgui.h>

void PlayerCollisionInfo::Init() {
    SetIsAbleCollision(false);
    currentLoopCount_ = 0;
    isInLoopWait_     = false;
    loopWaitTimer_    = 0.0f;
    BaseAABBCollisionBox::Init();
}

void PlayerCollisionInfo::Update() {
    if (!comboAttackData_) {
        return;
    }

    // プレイヤーを常に追従
    if (comboAttackData_->GetAttackParam().collisionParam.isAlwaysFollowing) {
        UpdateOffset();
    }

    // baseの更新
    BaseAABBCollisionBox::Update();
}

void PlayerCollisionInfo::TimerUpdate(const float& timeSpeed) {
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

void PlayerCollisionInfo::LoopWaiting(const float& timeSpeed) {
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

void PlayerCollisionInfo::LoopStart() {
    // コリジョンパラメータ取得
    const auto& collisionParam = comboAttackData_->GetAttackParam().collisionParam;

    isInLoopWait_ = false;
    adaptTimer_   = collisionParam.adaptTime;
    SetIsAbleCollision(true);
    UpdateOffset();
}

void PlayerCollisionInfo::AttackStart(const PlayerComboAttackData* comboAttackData) {
    comboAttackData_ = comboAttackData;

    // collision情報を取得
    const auto& collisionParam = comboAttackData_->GetAttackParam().collisionParam;

    // 初期パラメータセット
    adaptTimer_       = collisionParam.adaptTime;
    currentLoopCount_ = 0;
    loopWaitTimer_    = 0.0f;
    isInLoopWait_     = false;
    isFinish_         = false;
    isHit_            = false;

    // サイズセット
    SetSize(collisionParam.size);

    // collision位置Offset
    UpdateOffset();

    // コリジョン可能に
    SetIsAbleCollision(true);
}

void PlayerCollisionInfo::UpdateOffset() {
    Vector3 offSetValue        = comboAttackData_->GetAttackParam().collisionParam.offsetPos;
    Vector3 forwardOffSetValue = baseTransform_->CalcForwardOffset(offSetValue);
    SetOffset(forwardOffSetValue);
}

void PlayerCollisionInfo::OnCollisionStay([[maybe_unused]] BaseCollider* other) {
    if (dynamic_cast<BaseEnemy*>(other)) {
        isHit_ = true;
    }
}

void PlayerCollisionInfo::SetPlayerBaseTransform(const WorldTransform* playerBaseTransform) {
    baseTransform_ = playerBaseTransform;
}

Vector3 PlayerCollisionInfo::GetCollisionPos() const {
    return BaseAABBCollisionBox::GetCollisionPos();
}

void PlayerCollisionInfo::SetParentTransform(WorldTransform* transform) {
    BaseAABBCollisionBox::SetParentTransform(transform);
}