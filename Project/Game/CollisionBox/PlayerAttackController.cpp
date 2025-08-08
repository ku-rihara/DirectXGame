#include "PlayerAttackController.h"
// enemy
#include "Enemy/BaseEnemy.h"
#include <imgui.h>

void PlayerAttackController::Init() {
    ///* グローバルパラメータ
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_, false);
    BindParams();
    globalParameter_->SyncParamForGroup(groupName_);

    BaseAABBCollisionBox::Init();
}

void PlayerAttackController::Update() {
    BaseAABBCollisionBox::Update();
}

void PlayerAttackController::Draw() {
    BaseAABBCollisionBox::Draw();
}

///=========================================================
/// バインド
///==========================================================
void PlayerAttackController::BindParams() {
   
    for (int32_t i = 0; i < kComboLevel; ++i) {
        globalParameter_->Bind(groupName_, "AttackSpeedForLevel" + std::to_string(int(i + 1)), &attackValueForLevel_.speed[i]);
        globalParameter_->Bind(groupName_, "attackPowerForLevel" + std::to_string(int(i + 1)), &attackValueForLevel_.power[i]);
    }
}

///=========================================================
/// パラメータ調整
///==========================================================
void PlayerAttackController::AdjustParam() {

#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        // attackLevel Values
        for (int32_t i = 0; i < kComboLevel; ++i) {
            ImGui::SeparatorText(("AttackValue" + std::to_string(int(i + 1))).c_str());
            ImGui::DragFloat(("AttackSpeedForLevel" + std::to_string(int(i + 1))).c_str(), &attackValueForLevel_.speed[i], 0.01f);
            ImGui::DragFloat(("attackPowerForLevel" + std::to_string(int(i + 1))).c_str(), &attackValueForLevel_.power[i], 0.01f);
        }

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
#endif // _DEBUG
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
            /// PlayerAttackController::AttackType::STOPPER
            /// -----------------------------------------------------------------------
        case PlayerAttackController::AttackType::STOPPER:

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

Vector3 PlayerAttackController::GetCollisionPos() const {
    return BaseAABBCollisionBox::GetCollisionPos();
}

void PlayerAttackController::SetSize(const Vector3& size) {
    BaseAABBCollisionBox::SetSize(size);
}

void PlayerAttackController::SetPosition(const Vector3& position) {
    BaseAABBCollisionBox::SetPosition(position);
}

void PlayerAttackController::SetOffset(const Vector3& offset) {
    BaseAABBCollisionBox::SetOffset(offset);
}

void PlayerAttackController::IsAdapt(bool is) {
    BaseAABBCollisionBox::IsAdapt(is);
}