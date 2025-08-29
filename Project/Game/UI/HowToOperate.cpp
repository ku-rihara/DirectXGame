#include "HowToOperate.h"
#include "base/TextureManager.h"
#include"Frame/Frame.h"
#include <imgui.h>

void HowToOperate::Init() {
    uint32_t handle = TextureManager::GetInstance()->LoadTexture("Resources/Texture/HowToOperate.png");

    sprite_.reset(Sprite::Create(handle, {}, {1, 1, 1, 1}));
    sprite_->SetScale(Vector2::ZeroVector());
    sprite_->anchorPoint_ = {0.5f, 0.5f};
    position_             = {1033, 509};
    scale_                = Vector2::ZeroVector();

    scaleEasing_.Init("HowToOperatorScaling","HowToOperatorEasing.json");
    scaleEasing_.SetAdaptValue(&scale_);
    scaleEasing_.Reset();
}

void HowToOperate::Update() {
    sprite_->SetPosition(position_);
    sprite_->SetScale(scale_);
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