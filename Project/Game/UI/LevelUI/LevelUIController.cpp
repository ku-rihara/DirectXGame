#include "LevelUIController.h"
#include <imgui.h>

void LevelUIController::Init() {
    // グローバルパラメータ
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    // 数字UI初期化
    for (int32_t i = 0; i < static_cast<int32_t>(LevelUI::Digit::Count); ++i) {
        digits_[i] = std::make_unique<LevelUI>();
        digits_[i]->Init(static_cast<LevelUI::Digit>(i));
    }

    labelSprite_.reset(KetaEngine::Sprite::Create("UI/LevelUI.dds", true));
    if (labelSprite_) {
        labelSprite_->SetAnchorPoint({0.5f, 0.5f});
        labelSprite_->transform_.scale = {0.0f, 0.0f};
    }
}

void LevelUIController::Update(int32_t level) {
    for (int32_t i = 0; i < static_cast<int32_t>(LevelUI::Digit::Count); ++i) {
        Vector2 pos = basePosition_ + Vector2(digitOffset_.x * i, digitOffset_.y * i);
        digits_[i]->Update(baseScale_, alpha_);
        digits_[i]->CalculateNumber(level);
        digits_[i]->SetPosition(pos);
    }

    if (labelSprite_) {
        labelSprite_->transform_.pos   = basePosition_ + labelOffset_;
        labelSprite_->transform_.scale = baseScale_;
        labelSprite_->SetAlpha(alpha_);
    }
}

void LevelUIController::RegisterParams() {
    globalParameter_->Regist(groupName_, "basePosition", &basePosition_);
    globalParameter_->Regist(groupName_, "baseScale",    &baseScale_);
    globalParameter_->Regist(groupName_, "digitOffset",  &digitOffset_);
    globalParameter_->Regist(groupName_, "labelOffset",  &labelOffset_);
}

///=========================================================
/// パラメータ調整
///==========================================================
void LevelUIController::AdjustParam() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());
        ImGui::DragFloat2("Base Position", &basePosition_.x, 0.1f);
        ImGui::DragFloat2("Base Scale",    &baseScale_.x,    0.01f);
        ImGui::DragFloat2("Digit Offset",  &digitOffset_.x,  0.1f);
        ImGui::DragFloat2("Label Offset",  &labelOffset_.x,  0.1f);
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);
        ImGui::PopID();
    }
    for (auto& d : digits_) {
        d->AdjustParam();
    }
#endif
}
