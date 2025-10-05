#include "HowToOperate.h"
#include "base/TextureManager.h"
#include "Frame/Frame.h"
#include <imgui.h>

void HowToOperate::Init() {
  
    sprite_.reset(Sprite::Create("HowToOperate.png", true));

    scaleEasing_.Init("HowToOperatorScaling", "HowToOperatorEasing.json");
    scale_ = Vector2::ZeroVector();
    scaleEasing_.SetAdaptValue(&scale_);
    scaleEasing_.Reset();
}

void HowToOperate::Update() {
    sprite_->transform_.scale = scale_;
}

void HowToOperate::ScalingEasing() {
    scaleEasing_.Update(Frame::DeltaTime());
}

void HowToOperate::Draw() {
    sprite_->Draw();
}

void HowToOperate::Debug() {
    ImGui::SeparatorText("SpriteText");
    ImGui::DragFloat2("howtoope", &position_.x, 0.1f);
}