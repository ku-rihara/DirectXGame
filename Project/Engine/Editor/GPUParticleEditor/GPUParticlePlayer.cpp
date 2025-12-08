#include "GPUParticlePlayer.h"

void GPUParticlePlayer::Init() {
    BaseEffectPlayer::Init();
}

void GPUParticlePlayer::Update(float speedRate) {
    if (effectData_) {
        effectData_->Update(speedRate);
    }
}

void GPUParticlePlayer::Play(const std::string& particleName) {
    if (effectData_) {
        effectData_->Pause();
    }

    effectData_.reset();
    effectData_ = CreateEffectData();

    auto* particleData = dynamic_cast<GPUParticleData*>(effectData_.get());
    if (particleData) {
        particleData->Init(particleName);
        particleData->LoadData();
        particleData->Play();
    }

    currentEffectName_ = particleName;
    currentCategoryName_.clear();
}

void GPUParticlePlayer::PlayInCategory(const std::string& categoryName, const std::string& particleName) {
    if (effectData_) {
        effectData_->Pause();
    }

    effectData_.reset();
    effectData_ = CreateEffectData();

    auto* particleData = dynamic_cast<GPUParticleData*>(effectData_.get());
    if (particleData) {
        particleData->InitWithCategory(particleName, categoryName);
        particleData->LoadData();
        particleData->Play();
    }

    currentCategoryName_ = categoryName;
    currentEffectName_   = particleName;
}

void GPUParticlePlayer::Draw() {
    auto* particleData = dynamic_cast<GPUParticleData*>(effectData_.get());
    if (particleData) {
        particleData->Draw();
    }
}

std::unique_ptr<BaseEffectData> GPUParticlePlayer::CreateEffectData() {
    return std::make_unique<GPUParticleData>();
}

GPUParticleData* GPUParticlePlayer::GetParticleData() {
    return dynamic_cast<GPUParticleData*>(effectData_.get());
}