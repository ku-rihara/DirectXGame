#include "NumberDigitUI.h"

void NumberDigitUI::Init(const std::string& name) {
    sprite_.reset(KetaEngine::Sprite::Create("Number/Numbers.dds", true, name));
    if (sprite_) {
        sprite_->SetUVScale({kUVStep, 1.0f});
    }
}

void NumberDigitUI::Hide() {
    if (sprite_) {
        sprite_->SetIsDraw(false);
    }
}

void NumberDigitUI::SetColor(const Vector3& color) {
    if (sprite_) {
        sprite_->SetColor(color);
    }
}

void NumberDigitUI::Update(int32_t digit, const Vector2& pos, const Vector2& scale, float alpha, bool isVisible) {
    if (!sprite_) {
        return;
    }
    digit = std::clamp(digit, 0, 9);
    sprite_->SetIsDraw(isVisible);
    sprite_->transform_.pos   = pos;
    sprite_->transform_.scale = Vector2(scale.x * kUVStep, scale.y);
    sprite_->SetUVPosition(Vector2(static_cast<float>(digit) * kUVStep, 0.0f));
    sprite_->SetAlpha(alpha);
}
