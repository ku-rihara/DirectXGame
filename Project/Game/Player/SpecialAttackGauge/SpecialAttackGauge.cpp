#include "SpecialAttackGauge.h"
#include"base/TextureManager.h"
#include <cstdint>

void SpecialAttackGauge::Init() {

	int32_t gaugeFrameHandle = TextureManager::GetInstance()->LoadTexture("");
    int32_t gaugeHandle = TextureManager::GetInstance()->LoadTexture("");

	gaugeFrameSprite_.reset(Sprite::Create(gaugeFrameHandle, Vector2::UnitVector(), Vector4::kWHITE()));
    gaugeSprite_.reset(Sprite::Create(gaugeHandle, Vector2::UnitVector(), Vector4::kWHITE()));
}
void SpecialAttackGauge::Update() {


}

void SpecialAttackGauge::SpriteDraw() {

	gaugeFrameSprite_->Draw();
    gaugeSprite_->Draw();
}

void SpecialAttackGauge::AddGauge() {

}