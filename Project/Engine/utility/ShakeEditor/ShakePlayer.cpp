#include "ShakePlayer.h"

void ShakePlayer::Init() {
    totalShakeOffset_ = {0.0f, 0.0f, 0.0f};
    shakeData_        = std::make_unique<ShakeData>();
}

void ShakePlayer::Update(float deltaTime) {
    shakeData_->Update(deltaTime);
    // 合成シェイクオフセットを計算
    UpdateTotalShakeOffset();
}

void ShakePlayer::UpdateTotalShakeOffset() {
    totalShakeOffset_ = shakeData_->GetShakeOffset();
}

void ShakePlayer::Play(const std::string& shakeName) {
    shakeData_->Init(shakeName);
    shakeData_->Play();
}

void ShakePlayer::StopShake() {
    shakeData_->Stop();
    totalShakeOffset_ = {0.0f, 0.0f, 0.0f};
}
