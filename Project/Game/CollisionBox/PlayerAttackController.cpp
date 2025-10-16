#include "PlayerAttackController.h"
#include "Frame/Frame.h"
// enemy
#include "Enemy/BaseEnemy.h"
#include <imgui.h>

void PlayerAttackController::Init() {
    // グローバルパラメータ
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_, false);
    BindParams();
    globalParameter_->SyncParamForGroup(groupName_);

    SetIsCollision(false);

    BaseAABBCollisionBox::Init();
}

void PlayerAttackController::Update() {

    // 攻撃タイプのColliderパラメータ
    SetSize(attackParam_[static_cast<size_t>(attackType_)].collisionSize);

    // タイム更新
    TimerUpdate(Frame::DeltaTime());

    // baseの更新
    BaseAABBCollisionBox::Update();
}

void PlayerAttackController::Draw() {
    BaseAABBCollisionBox::Draw();
}

void PlayerAttackController::TimerUpdate(const float& deltaTime) {
    if (!isCollision_) {
        return;
    }
    // タイマー更新
    adaptTimer_ -= deltaTime;

    if (adaptTimer_ <= 0.0f) {
        SetIsCollision(false);
    }
}

void PlayerAttackController::ChangeAttackType(const AttackType& attackType) {
    attackType_ = attackType;

    // offset
    float offSetValue = attackParam_[static_cast<size_t>(attackType_)].collisionOffsetValue;
    SetOffset(baseTransform_->LookAt(Vector3::ToForward()) * offSetValue);

    // time
    adaptTimer_ = attackParam_[static_cast<size_t>(attackType_)].adaptTime;

    // isCollision
    SetIsCollision(true);
}

///==========================================================
/// バインド
///==========================================================
void PlayerAttackController::BindParams() {

    for (int32_t i = 0; i < kComboLevel; ++i) {
        globalParameter_->Bind(groupName_, "AttackSpeedRate" + std::to_string(int(i + 1)), &attackValueForLevel_[i].speedRate);
        globalParameter_->Bind(groupName_, "AttackPowerRate" + std::to_string(int(i + 1)), &attackValueForLevel_[i].powerRate);
    }

    for (int32_t i = 0; i < attackParam_.size(); ++i) {
        std::string attackTypeName = GetAttackTypeName(static_cast<AttackType>(i));
        globalParameter_->Bind(groupName_, "offsetValue_" + attackTypeName, &attackParam_[i].collisionOffsetValue);
        globalParameter_->Bind(groupName_, "ColliderSize_" + attackTypeName, &attackParam_[i].collisionSize);
        globalParameter_->Bind(groupName_, "adaptTime_" + attackTypeName, &attackParam_[i].adaptTime);
        globalParameter_->Bind(groupName_, "power_" + attackTypeName, &attackParam_[i].power);
    }
}

///==========================================================
/// パラメータ調整
///==========================================================
void PlayerAttackController::AdjustParam() {

#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

         ImGui::Text("CurrentLevel:%d", pCombo_->GetCurrentLevel());

        // attackLevel Values
        for (int32_t i = 0; i < kComboLevel; ++i) {
            ImGui::SeparatorText(("AttackValue" + std::to_string(int(i + 1))).c_str());
            ImGui::DragFloat(("AttackSpeedRate" + std::to_string(int(i + 1))).c_str(), &attackValueForLevel_[i].speedRate, 0.01f);
            ImGui::DragFloat(("attackPowerRate" + std::to_string(int(i + 1))).c_str(), &attackValueForLevel_[i].powerRate, 0.01f);
        }

        for (int32_t i = 0; i < attackParam_.size(); ++i) {
            ImGui::SeparatorText(("AttackParameter" + GetAttackTypeName(static_cast<AttackType>(i))).c_str());

            ImGui::DragFloat3(("CollisionSize" + std::to_string(int(i + 1))).c_str(), &attackParam_[i].collisionSize.x, 0.01f);
            ImGui::DragFloat(("offsetValue" + std::to_string(int(i + 1))).c_str(), &attackParam_[i].collisionOffsetValue, 0.01f);
            ImGui::DragFloat(("adaptTime" + std::to_string(int(i + 1))).c_str(), &attackParam_[i].adaptTime, 0.01f);
            ImGui::DragFloat(("power" + std::to_string(int(i + 1))).c_str(), &attackParam_[i].power, 0.01f);
        }

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
#endif // _DEBUG
}

// AttackTypeを文字列に変換する関数
std::string PlayerAttackController::GetAttackTypeName(const AttackType& type) {
    switch (type) {
    case AttackType::NORMAL:
        return "NORMAL";
    case AttackType::UPPER:
        return "UPPER";
    case AttackType::THRUST:
        return "THRUST";
    case AttackType::FALL:
        return "FALL";
    case AttackType::RUSH:
        return "RUSH";
    default:
        return "UNKNOWN";
    }
}

float PlayerAttackController::GetAttackPower() {

    // 攻撃力と上昇値を取得
    float value = attackParam_[static_cast<size_t>(attackType_)].power;
    float rate  = attackValueForLevel_[pCombo_->GetCurrentLevel()].powerRate;

    // 実際の攻撃力
    float result = value * rate;

    return result;
}

float PlayerAttackController::GetAttackSpeed(const float& baseTime) {

    float result = baseTime * attackValueForLevel_[pCombo_->GetCurrentLevel()].speedRate;

    return result;
}

void PlayerAttackController::OnCollisionEnter([[maybe_unused]] BaseCollider* other) {
    if (dynamic_cast<BaseEnemy*>(other)) {
        switch (attackType_) {
            /// -----------------------------------------------------------------------
            /// PlayerAttackController::AttackType::NORMAL
            /// -----------------------------------------------------------------------
        case PlayerAttackController::AttackType::NORMAL:

            break;
        }
    }
}

void PlayerAttackController::OnCollisionStay([[maybe_unused]] BaseCollider* other) {
    if (dynamic_cast<BaseEnemy*>(other)) {
        switch (attackType_) {

            /// -----------------------------------------------------------------------
            /// PlayerAttackController::AttackType::FALL
            /// -----------------------------------------------------------------------
        case PlayerAttackController::AttackType::FALL:

            break;
            /// -----------------------------------------------------------------------
            /// PlayerAttackController::AttackType::RUSH
            /// -----------------------------------------------------------------------
        case PlayerAttackController::AttackType::RUSH:

            break;

            /// -----------------------------------------------------------------------
            /// PlayerAttackController::AttackType::THRUST
            /// -----------------------------------------------------------------------
        case PlayerAttackController::AttackType::THRUST:
            isSlow_ = true;
            break;
            /// -----------------------------------------------------------------------
            /// PlayerAttackController::AttackType::UPPER
            /// -----------------------------------------------------------------------
        case PlayerAttackController::AttackType::UPPER:
            isHitStop_ = true;
            break;
            /// -----------------------------------------------------------------------
            /// PlayerAttackController::AttackType::COUNT
            /// -----------------------------------------------------------------------
        case PlayerAttackController::AttackType::COUNT:
            break;
        default:
            break;
        }
    }
}

void PlayerAttackController::ResetComboEffect() {
    isSlow_    = false;
    isHitStop_ = false;
}

void PlayerAttackController::SetCombo(Combo* combo) {
    pCombo_ = combo;
}

void PlayerAttackController::SetPlayerBaseTransform(const WorldTransform* playerBaseTransform) {
    baseTransform_ = playerBaseTransform;
}

Vector3 PlayerAttackController::GetCollisionPos() const {
    return BaseAABBCollisionBox::GetCollisionPos();
}

void PlayerAttackController::SetParentTransform(WorldTransform* transform) {
    BaseAABBCollisionBox::SetParentTransform(transform);
}
