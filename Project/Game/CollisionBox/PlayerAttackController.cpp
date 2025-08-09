#include "PlayerAttackController.h"
// enemy
#include "Enemy/BaseEnemy.h"
#include <imgui.h>

void PlayerAttackController::Init() {
    // グローバルパラメータ
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_, false);
    BindParams();
    globalParameter_->SyncParamForGroup(groupName_);

    BaseAABBCollisionBox::Init();
}

void PlayerAttackController::Update() {

    // 攻撃タイプのColliderパラメータ
    SetSize(collisionParam_[static_cast<size_t>(attackType_)].size);
    BaseAABBCollisionBox::Update();
}

void PlayerAttackController::Draw() {
    BaseAABBCollisionBox::Draw();
}

///==========================================================
/// バインド
///==========================================================
void PlayerAttackController::BindParams() {

    for (int32_t i = 0; i < kComboLevel; ++i) {
        globalParameter_->Bind(groupName_, "AttackSpeedForLevel" + std::to_string(int(i + 1)), &attackValueForLevel_[i].speed);
        globalParameter_->Bind(groupName_, "attackPowerForLevel" + std::to_string(int(i + 1)), &attackValueForLevel_[i].power);
    }

    for (int32_t i = 0; i < collisionParam_.size(); ++i) {
        globalParameter_->Bind(groupName_, "offsetValue" + std::to_string(int(i + 1)), &collisionParam_[i].offsetValue);
        globalParameter_->Bind(groupName_, "ColliderSize" + std::to_string(int(i + 1)), &collisionParam_[i].size);
        globalParameter_->Bind(groupName_, "aliveTime" + std::to_string(int(i + 1)), &collisionParam_[i].aliveTime);
    }
}

///==========================================================
/// パラメータ調整
///==========================================================
void PlayerAttackController::AdjustParam() {

#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        // attackLevel Values
        for (int32_t i = 0; i < kComboLevel; ++i) {
            ImGui::SeparatorText(("AttackValue" + std::to_string(int(i + 1))).c_str());
            ImGui::DragFloat(("AttackSpeedForLevel" + std::to_string(int(i + 1))).c_str(), &attackValueForLevel_[i].speed, 0.01f);
            ImGui::DragFloat(("attackPowerForLevel" + std::to_string(int(i + 1))).c_str(), &attackValueForLevel_[i].power, 0.01f);
        }

        for (int32_t i = 0; i < collisionParam_.size(); ++i) {
            ImGui::SeparatorText(("ColliderParameter" + std::to_string(int(i + 1))).c_str());
            ImGui::DragFloat3(("CollisionSize" + std::to_string(int(i + 1))).c_str(), &collisionParam_[i].size.x, 0.01f);
            ImGui::DragFloat(("offsetValue" + std::to_string(int(i + 1))).c_str(), &collisionParam_[i].offsetValue, 0.01f);
            ImGui::DragFloat(("aliveTime" + std::to_string(int(i + 1))).c_str(), &collisionParam_[i].aliveTime, 0.01f);
        }

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
#endif // _DEBUG
}

float PlayerAttackController::GetAttackValue(AttackValueMode attackValueMode) {
    float attackValue = 0.0f;

    switch (attackValueMode) {
        /// ---------------------------------------------------------------------------------------
        /// アタックスピード
        ///----------------------------------------------------------------------------------------
    case PlayerAttackController::AttackValueMode::AttackSpeed:
        attackValue = attackValueForLevel_[pCombo_->GetCurrentLevel()].speed;
        break;
        /// ---------------------------------------------------------------------------------------
        /// アタックパワー
        ///----------------------------------------------------------------------------------------
    case PlayerAttackController::AttackValueMode::AttackPower:
        attackValue = attackValueForLevel_[pCombo_->GetCurrentLevel()].power;
        break;
    default:
        break;
    }
    // 値を返す
    return attackValue;
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

void PlayerAttackController::SetCombo(Combo* combo) {
    pCombo_ = combo;
}

void PlayerAttackController::ChangeAttackType(AttackType attackType) {
    attackType_ = attackType;
}

void PlayerAttackController::SetPlayerBaseTransform(const WorldTransform* playerBaseTransform) {
    playerBaseTransform_ = playerBaseTransform;
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

void PlayerAttackController::SetParentTransform(WorldTransform* transform) {
    BaseAABBCollisionBox::SetParentTransform(transform);
}

