#include "GPUParticlePlayer.h"

using namespace KetaEngine;

void GPUParticlePlayer::Init() {
    BaseEffectPlayer::Init();
}

void GPUParticlePlayer::Update(float speedRate) {
    if (effectData_) {
        effectData_->Update(speedRate);

        // 連続エミットモード
        if (isContinuousEmit_) {
            auto* particleData = GetParticleData();
            if (particleData) {
                particleData->Draw();
            }
        }
        // 一度だけエミットするモード
        else if (shouldEmitOnce_) {
            auto* particleData = GetParticleData();
            if (particleData) {
                particleData->Draw();
            }
            shouldEmitOnce_ = false;
        }
    }
}

void GPUParticlePlayer::Play(const std::string& categoryName, const std::string& particleName) {
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

    // 連続エミットモードを有効化
    isContinuousEmit_ = true;

    // パーティクルデータの再生状態を確認
    auto* particleData = GetParticleData();
    if (particleData && !particleData->IsPlaying()) {
        particleData->Play();
    }
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
