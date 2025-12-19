#include "SideRopeController.h"
#include <imgui.h>

void SideRopeController::Init() {

    // グローバルパラメータ
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    // 生成と初期化
    for (int32_t i = 0; i < static_cast<int32_t>(SideRopeType::COUNT); ++i) {
        sideRopes_[i] = std::make_unique<SideRope>();
        sideRopes_[i]->Init(i);
        sideRopes_[i]->SetRopeParam(ropeParam_);
    }
}

void SideRopeController::Update() {

    for (auto& sideRope : sideRopes_) {
        sideRope->Update();
    }
}

void SideRopeController::RegisterParams() {
    globalParameter_->Regist<float>(groupName_, "maxStretch", &ropeParam_.maxStretch);
    globalParameter_->Regist<float>(groupName_, "stretchResistance", &ropeParam_.stretchResistance);
    globalParameter_->Regist<float>(groupName_, "reboundRate", &ropeParam_.reboundRate);
    globalParameter_->Regist<float>(groupName_, "delayTime", &ropeParam_.delayTime);
}

void SideRopeController::AdjustParam() {
#ifdef _DEBUG

    for (auto& sideRope : sideRopes_) {
        sideRope->AdjustParam();
    }

    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        /// Collision
        ImGui::SeparatorText("BoundParam");
        ImGui::DragFloat("maxStretch", &ropeParam_.maxStretch, 0.1f);
        ImGui::DragFloat("stretchResistance", &ropeParam_.stretchResistance, 0.1f);
        ImGui::DragFloat("reboundRate", &ropeParam_.reboundRate, 0.1f);
        ImGui::DragFloat("delayTime", &ropeParam_.delayTime, 0.1f);

        /// セーブとロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }

#endif // _DEBUG
}
