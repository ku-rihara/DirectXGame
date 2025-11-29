#include "PlayerCollisionInfo.h"
#include "Frame/Frame.h"
#include "Player/ComboCreator/PlayerComboAttackData.h"
// enemy
#include "Enemy/Types/BaseEnemy.h"
#include <imgui.h>

void PlayerCollisionInfo::Init() {

    SetIsCollision(false);
    BaseAABBCollisionBox::Init();
}

void PlayerCollisionInfo::Update(const float&timeSpeed) {

    // タイム更新
    TimerUpdate(timeSpeed);

    // baseの更新
    BaseAABBCollisionBox::Update();
}

void PlayerCollisionInfo::TimerUpdate(const float& timeSpeed) {
    if (!isCollision_) {
        return;
    }
    // タイマー更新
    adaptTimer_ -= timeSpeed;

    if (adaptTimer_ <= 0.0f) {
        SetIsCollision(false);
    }
}

void PlayerCollisionInfo::AttackStart(const PlayerComboAttackData* comboAttackData) {
    comboAttackData_ = comboAttackData;

    // collision情報を取得
    adaptTimer_ = comboAttackData_->GetAttackParam().collisionParam.adaptTime;
    SetSize(comboAttackData_->GetAttackParam().collisionParam.size);

    // collision位置Offset
    UpdateOffset();

    // コリジョン可能に 
    SetIsCollision(true);
}

void PlayerCollisionInfo::UpdateOffset() {
    Vector3 offSetValue        = comboAttackData_->GetAttackParam().collisionParam.offsetPos;
    Vector3 forwardOffSetValue = baseTransform_->CalcForwardOffset(offSetValue);
    SetOffset(forwardOffSetValue);
}

void PlayerCollisionInfo::OnCollisionStay([[maybe_unused]] BaseCollider* other) {
    if (dynamic_cast<BaseEnemy*>(other)) {
        isColliding_ = true;
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
