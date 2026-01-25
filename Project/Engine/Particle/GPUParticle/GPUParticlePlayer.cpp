#include "GPUParticlePlayer.h"

using namespace KetaEngine;

void GPUParticlePlayer::Init() {
    BaseEffectPlayer::Init();
    isInitialized_          = false;
    playRequestedThisFrame_ = false;
}

void GPUParticlePlayer::InitEffect(const std::string& categoryName, const std::string& particleName) {
    // エフェクトデータを作成してロード
    effectData_ = CreateEffectData();

    auto* particleData = dynamic_cast<GPUParticleData*>(effectData_.get());
    if (particleData) {
        particleData->Init(particleName, categoryName);
        particleData->LoadData();

        currentCategoryName_ = categoryName;
        currentEffectName_   = particleName;
        isInitialized_       = true;
    }
}

void GPUParticlePlayer::Update(float speedRate) {
    if (!effectData_ || !isInitialized_) {
        return;
    }

    auto* particleData = GetParticleData();
    if (!particleData) {
        return;
    }

    // 今フレームにPlay()が呼ばれていない場合、自動的に停止
    if (!playRequestedThisFrame_) {
        if (particleData->IsPlaying()) {
            particleData->Reset();
        }
    }

    // 次フレーム用にリセット
    playRequestedThisFrame_ = false;

    // エフェクトの更新
    effectData_->Update(speedRate);
}

void GPUParticlePlayer::Play(const std::string& categoryName, const std::string& particleName) {
    // 今フレームにPlay()が呼ばれたことをマーク
    playRequestedThisFrame_ = true;

    // 初期化されていない、または異なるエフェクトの場合
    if (!isInitialized_ || currentCategoryName_ != categoryName || currentEffectName_ != particleName) {

        if (effectData_) {
            effectData_->Pause();
            effectData_.reset();
        }

        InitEffect(categoryName, particleName);
    }

    // エフェクトを再生
    auto* particleData = GetParticleData();
    if (particleData) {
        if (!particleData->IsPlaying()) {
            particleData->Play();
        }
    }
}

void GPUParticlePlayer::Reset() {
    auto* particleData = GetParticleData();
    if (particleData) {
        particleData->Reset();
    }
    playRequestedThisFrame_ = false;
}

void GPUParticlePlayer::Draw() {
    if (!isInitialized_) {
        return;
    }

    auto* particleData = GetParticleData();
    if (particleData) {
        particleData->Draw();
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