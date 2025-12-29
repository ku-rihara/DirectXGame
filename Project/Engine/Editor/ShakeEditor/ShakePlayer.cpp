#include "ShakePlayer.h"

using namespace KetaEngine;

void ShakePlayer::Init() {
    BaseEffectPlayer::Init();
    totalShakeOffset_ = {0.0f, 0.0f, 0.0f};
}

void ShakePlayer::Update(float speedRate) {
    if (effectData_) {
        effectData_->Update(speedRate);
        UpdateTotalShakeOffset();
    }
}

void ShakePlayer::UpdateTotalShakeOffset() {
    if (ShakeData* shakeData = dynamic_cast<ShakeData*>(effectData_.get())) {
        totalShakeOffset_ = shakeData->GetShakeOffset();
    }
}

void ShakePlayer::Play(const std::string& shakeName) {
    currentEffectName_ = shakeName;

    effectData_.reset();
    effectData_ = CreateEffectData();

    effectData_->Init(shakeName);
    effectData_->LoadData();
    effectData_->Play();
}

std::unique_ptr<BaseEffectData> ShakePlayer::CreateEffectData() {
    return std::make_unique<ShakeData>();
}
