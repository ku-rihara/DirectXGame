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

void PlayerCollisionInfo::Update() {

    // タイム更新
    TimerUpdate(Frame::DeltaTime());

    // baseの更新
    BaseAABBCollisionBox::Update();
}

void PlayerCollisionInfo::TimerUpdate(const float& deltaTime) {
    if (!isCollision_) {
        return;
    }
    // タイマー更新
    adaptTimer_ -= deltaTime;

    if (adaptTimer_ <= 0.0f) {
        SetIsCollision(false);
    }
}

// void PlayerCollisionInfo::ChangeAttackType(const AttackType& attackType) {
//     attackType_ = attackType;
//
//     // offset
//     float offSetValue = attackParam_[static_cast<size_t>(attackType_)].collisionOffsetValue;
//     SetOffset(baseTransform_->LookAt(Vector3::ToForward()) * offSetValue);
//
//     // time
//     adaptTimer_ = attackParam_[static_cast<size_t>(attackType_)].adaptTime;
//
//     // isCollision
//     SetIsCollision(true);
// }
//

// float PlayerCollisionInfo::GetAttackSpeed(const float& baseTime) {
//
//     float result = baseTime * attackValueForLevel_[pCombo_->GetCurrentLevel()].speedRate;
//
//     return result;
// }

void PlayerCollisionInfo::GetAttackInfo(const PlayerComboAttackData* comboAttackData) {
    comboAttackData_ = comboAttackData;

    // collision情報を取得
    adaptTimer_ = comboAttackData_->GetAttackParam().collisionParam.adaptTime;
    SetSize(comboAttackData_->GetAttackParam().collisionParam.size);

    // collision位置Offset
}

void PlayerCollisionInfo::UpdateOffset() {
    Vector3 offSetValue = comboAttackData_->GetAttackParam().collisionParam.offsetPos;
    SetOffset(baseTransform_->LookAt(Vector3::ToForward()) * offSetValue);
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
