#include "HowToOperate.h"
#include "base/TextureManager.h"
#include "Frame/Frame.h"
#include <imgui.h>

void HowToOperate::Init() {

    sprite_.reset(KetaEngine::Sprite::Create("HowToOperate.png", true));
    sprite_->transform_.scale = Vector2::ZeroVector();
}

void HowToOperate::Update() {
}

void HowToOperate::SetScale(const Vector2& scale) {
    sprite_->transform_.scale = scale;
}

void HowToOperate::Draw() {
    sprite_->Draw();
}

void HowToOperate::Debug() {
    ImGui::SeparatorText("SpriteText");
    ImGui::DragFloat2("howtoope", &position_.x, 0.1f);
}
