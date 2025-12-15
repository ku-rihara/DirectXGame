#include "PlayerHandLeft.h"
/// imgui
#include <imgui.h>

///=========================================================
/// 　初期化
///==========================================================
void PlayerHandLeft::Init() {

    // グループネーム
    groupName_     = "LeftHand";
    railGroupName_ = "ThrustRail";

    obj3d_.reset(KetaEngine::Object3d::CreateModel("LHand.obj"));
    obj3d_->transform_.scale_                       = {2, 2, 2};
    obj3d_->material_.materialData_->enableLighting = 2;
    obj3d_->material_.SetEnvironmentCoefficient(0.15f);
   
    trustRailManager_ = std::make_unique<KetaEngine::RailManager>();
    trustRailManager_->Init(railGroupName_);

    BasePlayerHand::Init();

    gpuParticlePlayer_.Init();
    gpuParticlePlayer_.InitEffect("Player", "LeftHandParticle");
}

///=========================================================
/// 　更新
///==========================================================
void PlayerHandLeft::Update() {
    gpuParticlePlayer_.Play("Player", "LeftHandParticle");
    gpuParticlePlayer_.SetEmitPosition(GetWorldPosition());
    gpuParticlePlayer_.Update();
    BasePlayerHand::Update();
}


///=====================================================
///  パラメータ調節
///=====================================================
void PlayerHandLeft::AdjustParam() {

    BasePlayerHand::SetValues();

    if (ImGui::CollapsingHeader("LeftHand")) {
        ImGui::PushID("LeftHand");
        BasePlayerHand::AdjustParamBase();
        trustRailManager_->ImGuiEdit();
        SaveAndLoad();
        ImGui::PopID();
    }
}

void PlayerHandLeft::DissolveAdapt(float dissolve) {
    BasePlayerHand::DissolveAdapt(dissolve);
}

///=====================================================
///  セーブロード
///=====================================================
void PlayerHandLeft::SaveAndLoad() {
    BasePlayerHand::SaveAndLoad();
}

void PlayerHandLeft::SetParent(KetaEngine::WorldTransform* parent) {
    BasePlayerHand::SetParent(parent);
}

void PlayerHandLeft::SetRailParent(KetaEngine::WorldTransform* parent) {
    trustRailManager_->SetParent(parent);
}