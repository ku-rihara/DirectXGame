#include "ComboChainGauge.h"
#include <algorithm>
#include <imgui.h>

void ComboChainGauge::Init() {

    // グローバルパラメータ
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    // スプライト生成
    gaugeSprite_.reset(KetaEngine::Sprite::Create("UI/ComboChainGauge.dds", true));
    if (gaugeSprite_) {
        gaugeSprite_->SetAnchorPoint({0.0f, 0.5f});
        gaugeSprite_->SetIsDraw(false);
    }
}

void ComboChainGauge::Update(float comboTime, float comboMaxTime, float alpha) {
    if (!gaugeSprite_) {
        return;
    }

    // コンボが非アクティブなら非表示
    if (comboTime <= 0.0f || comboMaxTime <= 0.0f) {
        gaugeSprite_->SetIsDraw(false);
        return;
    }

    // 残り時間の割合 (1.0 = 満タン, 0.0 = 空)
    const float rate = std::clamp(comboTime / comboMaxTime, 0.0f, 1.0f);

    gaugeSprite_->SetIsDraw(true);
    gaugeSprite_->SetGaugeRate(rate);
    gaugeSprite_->transform_.pos   = position_;
    gaugeSprite_->transform_.scale = scale_;
    gaugeSprite_->SetColor(color_);
    gaugeSprite_->SetAlpha(alpha);
}

///==========================================================
/// パラメータ調整
///==========================================================
void ComboChainGauge::AdjustParam() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::DragFloat2("Position", &position_.x, 1.0f);
        ImGui::DragFloat2("Scale", &scale_.x, 0.01f);
        ImGui::ColorEdit3("Color", &color_.x);

        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
#endif
}

///==========================================================
/// パラメータBind
///==========================================================
void ComboChainGauge::RegisterParams() {
    globalParameter_->Regist(groupName_, "position", &position_);
    globalParameter_->Regist(groupName_, "scale", &scale_);
    globalParameter_->Regist(groupName_, "color", &color_);
}
