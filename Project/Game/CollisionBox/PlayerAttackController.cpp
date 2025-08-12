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
    SetSize(collisionParam_[static_cast<size_t>(attackType_)].size);

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

void PlayerAttackController::ChangeAttackType(AttackType attackType) {
    attackType_ = attackType;

    // offset
    float offSetValue = collisionParam_[static_cast<size_t>(attackType_)].offsetValue;
    SetOffset(baseTransform_->LookAt(Vector3::ToForward()) * offSetValue);

    // time
    adaptTimer_ = collisionParam_[static_cast<size_t>(attackType_)].adaptTime;

    // isCollision
    SetIsCollision(true);
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
        std::string attackTypeName = GetAttackTypeName(static_cast<AttackType>(i));
        globalParameter_->Bind(groupName_, "offsetValue_" + attackTypeName, &collisionParam_[i].offsetValue);
        globalParameter_->Bind(groupName_, "ColliderSize_" + attackTypeName, &collisionParam_[i].size);
        globalParameter_->Bind(groupName_, "adaptTime_" +attackTypeName, &collisionParam_[i].adaptTime);
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
            ImGui::SeparatorText(("ColliderParameter_" + GetAttackTypeName(static_cast<AttackType>(i))).c_str());

            ImGui::DragFloat3(("CollisionSize" + std::to_string(int(i + 1))).c_str(), &collisionParam_[i].size.x, 0.01f);
            ImGui::DragFloat(("offsetValue" + std::to_string(int(i + 1))).c_str(), &collisionParam_[i].offsetValue, 0.01f);
            ImGui::DragFloat(("adaptTime" + std::to_string(int(i + 1))).c_str(), &collisionParam_[i].adaptTime, 0.01f);
        }

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
#endif // _DEBUG
}

// AttackTypeを文字列に変換する関数
std::string PlayerAttackController::GetAttackTypeName(AttackType type) {
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

void PlayerAttackController::SetPlayerBaseTransform(const WorldTransform* playerBaseTransform) {
    baseTransform_ = playerBaseTransform;
}

Vector3 PlayerAttackController::GetCollisionPos() const {
    return BaseAABBCollisionBox::GetCollisionPos();
}

void PlayerAttackController::SetParentTransform(WorldTransform* transform) {
    BaseAABBCollisionBox::SetParentTransform(transform);
}
