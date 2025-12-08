#include "PlayerHandRight.h"
/// imgui
#include <imgui.h>

///==========================================================
/// 　初期化
///==========================================================
void PlayerHandRight::Init() {

    // グループネーム
    groupName_      = "RightHand";
    threeComboName_ = "ThreeComboRail";
    forthComboName_ = "StopPunchRail";

    obj3d_.reset(Object3d::CreateModel("RHand.obj"));  
    obj3d_->material_.materialData_->enableLighting = 2;
    obj3d_->transform_.scale_                       = {2, 2, 2};
    obj3d_->material_.SetEnvironmentCoefficient(0.15f);
   
    /// rail
    threeComboRailManager_ = std::make_unique<RailManager>();
    threeComboRailManager_->Init(threeComboName_);

    stopPunchRailManager_ = std::make_unique<RailManager>();
    stopPunchRailManager_->Init(forthComboName_);

    BasePlayerHand::Init();
}

///=========================================================
/// 　更新
///==========================================================
void PlayerHandRight::Update() {
    stopPunchRailManager_->Update(0.1f, RailManager::PositionMode::LOCAL, (Vector3(1, 1, 1)));
    BasePlayerHand::Update();
}

void PlayerHandRight::RailThreeComboUpdate(float speed) {
    /// 3コンボレール更新
    threeComboRailManager_->Update(speed, RailManager::PositionMode::LOCAL, (Vector3(1, 1, 1)));
    obj3d_->transform_.translation_ = (threeComboRailManager_->GetPositionOnRail());
}

void PlayerHandRight::RailForthComboUpdate(float speed) {
    /// 4コンボレール更新
    stopPunchRailManager_->Update(speed, RailManager::PositionMode::LOCAL, (Vector3(1, 1, 1)));
    obj3d_->transform_.translation_ = (stopPunchRailManager_->GetPositionOnRail());
}

void PlayerHandRight::AdjustParam() {

    BasePlayerHand::SetValues();

    if (ImGui::CollapsingHeader("RightHand")) {
        ImGui::PushID("RightHand");
        BasePlayerHand::AdjustParamBase();
        threeComboRailManager_->ImGuiEdit();
        stopPunchRailManager_->ImGuiEdit();
        SaveAndLoad();
        ImGui::PopID();
    }
}

void PlayerHandRight::DissolveAdapt(float dissolve) {
    BasePlayerHand::DissolveAdapt(dissolve);
}

///=====================================================
///  セーブロード
///=====================================================
void PlayerHandRight::SaveAndLoad() {

    BasePlayerHand::SaveAndLoad();
}

void PlayerHandRight::SetParent(WorldTransform* parent) {
    BasePlayerHand::SetParent(parent);
}

void PlayerHandRight::SetRailParent(WorldTransform* parent) {
    threeComboRailManager_->SetParent(parent);
    stopPunchRailManager_->SetParent(parent);
}