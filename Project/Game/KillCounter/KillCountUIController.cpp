#include "KillCountUIController.h"
#include <cstdint>
#include <imgui.h>

void KillCountUIController::Init() {

    // グローバルパラメータ
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    // UI生成
    for (int32_t i = 0; i < static_cast<int32_t>(killCountSprites_.size()); ++i) {
        killCountSprites_[i] = std::make_unique<KillCountUI>();
        killCountSprites_[i]->Init(static_cast<KillCountUI::KillCountDigit>(i));
    }
}

void KillCountUIController::Update(int32_t killCount) {

    for (int32_t i = 0; i < static_cast<int32_t>(killCountSprites_.size()); ++i) {
        Vector2 digitPosition = basePosition_ + Vector2(digitOffset_.x * i, digitOffset_.y * i);

        killCountSprites_[i]->Update(baseScale_, alpha_);
        killCountSprites_[i]->CalculateNumber(killCount);
        killCountSprites_[i]->SetPosition(digitPosition);
        killCountSprites_[i]->SetRotateY(rotateY_);
    }
}

///=========================================================
/// パラメータ調整
///==========================================================
void KillCountUIController::AdjustParam() {
#ifdef _DEBUG

    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::SeparatorText("Parameter");

        ImGui::DragFloat2("Base Position", &basePosition_.x, 0.1f);
        ImGui::DragFloat2("Digit Offset", &digitOffset_.x, 0.1f);
        ImGui::DragFloat2("Base Scale", &baseScale_.x, 0.01f);
        ImGui::DragFloat("RotateY", &rotateY_, 0.01f);

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }

    for (int32_t i = 0; i < static_cast<int32_t>(killCountSprites_.size()); ++i) {
        killCountSprites_[i]->AdjustParam();
    }

#endif
}

///=========================================================
/// パラメータBind
///==========================================================
void KillCountUIController::RegisterParams() {
    globalParameter_->Regist(groupName_, "basePosition", &basePosition_);
    globalParameter_->Regist(groupName_, "digitOffset", &digitOffset_);
    globalParameter_->Regist(groupName_, "baseScale", &baseScale_);
    globalParameter_->Regist(groupName_, "rotateY", &rotateY_);
}
