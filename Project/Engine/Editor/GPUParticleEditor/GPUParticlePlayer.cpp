#include "GPUParticlePlayer.h"
#include "GPUParticle/GPUParticleManager.h"

void GPUParticlePlayer::Init() {
    BaseEffectPlayer::Init();
    isEmitting_     = false;
    shouldEmitOnce_ = false;
}

void GPUParticlePlayer::Update(float speedRate) {
    if (effectData_) {
        effectData_->Update(speedRate);

        // 連続エミット中または一度だけエミットする場合
        if (isEmitting_ || shouldEmitOnce_) {
            auto* particleData = GetParticleData();
            if (particleData) {
                particleData->Draw();
            }
            shouldEmitOnce_ = false;
        }
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
    // 初回または違うパーティクルの場合はロード
    if (!effectData_ || currentCategoryName_ != categoryName || currentEffectName_ != particleName) {
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

    // 毎回エミット
    auto* particleData = GetParticleData();
    if (particleData) {
        particleData->Draw();
    }
}


void GPUParticlePlayer::EmitOnce() {
    shouldEmitOnce_ = true;
}

void GPUParticlePlayer::StartEmit() {
    isEmitting_ = true;
}

void GPUParticlePlayer::StopEmit() {
    isEmitting_ = false;
}

void GPUParticlePlayer::SetEmitPosition(const Vector3& position) {
    auto* particleData = GetParticleData();
    if (!particleData) {
        return;
    }

    // 全セクションのエミット位置を設定
    for (int i = 0; i < particleData->GetTotalKeyFrameCount(); ++i) {
        auto* section = particleData->GetKeyFrame(i);
        if (section) {
            ParticleEmit emitterData = {};
            emitterData.translate    = position;
            section->SetEmitterData(emitterData);
        }
    }
}

void GPUParticlePlayer::SetParentTransform(WorldTransform* parent) {
    auto* particleData = GetParticleData();
    if (!particleData) {
        return;
    }

    // 全セクションに親トランスフォームを設定
    for (int i = 0; i < particleData->GetTotalKeyFrameCount(); ++i) {
        auto* section = particleData->GetKeyFrame(i);
        if (section) {
            section->SetParentBasePos(parent);
        }
    }
}



std::unique_ptr<BaseEffectData> GPUParticlePlayer::CreateEffectData() {
    return std::make_unique<GPUParticleData>();
}

GPUParticleData* GPUParticlePlayer::GetParticleData() {
    return dynamic_cast<GPUParticleData*>(effectData_.get());
}