#include "ParticlePlayer.h"

using namespace KetaEngine;

void ParticlePlayer::Init() {
    BaseEffectPlayer::Init();
    isInitialized_          = false;
    wasPlayCalledThisFrame_ = false;
}

void ParticlePlayer::Update(float speedRate) {
    // Play関数が呼ばれていたか確認
    UpdatePlayState();

    if (effectData_) {
        effectData_->Update(speedRate);
    }

    // フラグをリセット
    wasPlayCalledThisFrame_ = false;
}

void ParticlePlayer::UpdatePlayState() {
    // このフレームでPlayが呼ばれなかった場合、Pauseする
    if (isInitialized_ && !wasPlayCalledThisFrame_) {
        auto* particleData = GetParticleData();
        if (particleData && particleData->IsPlaying()) {
            particleData->Pause();
        }
    }
}

void ParticlePlayer::Play(const std::string& categoryName, const std::string& particleName) {
    // このフレームでPlayが呼ばれたことを記録
    wasPlayCalledThisFrame_ = true;

    // 初回呼び出し、またはパーティクル名が変わった場合のみ初期化
    if (!isInitialized_ || currentCategoryName_ != categoryName || currentParticleName_ != particleName) {

        // 既存のエフェクトを停止
        if (effectData_) {
            effectData_->Pause();
        }

        // 新しいエフェクトデータを作成
        effectData_.reset();
        effectData_ = CreateEffectData();

        auto* particleData = dynamic_cast<ParticleData*>(effectData_.get());
        if (particleData) {
            particleData->InitWithCategory(particleName, categoryName);
            particleData->LoadData();
            particleData->Play();
        }

        currentCategoryName_ = categoryName;
        currentParticleName_ = particleName;
        currentEffectName_   = particleName;
        isInitialized_       = true;
    } else {
        // 同じパーティクルで、Pause状態だった場合は再開
        auto* particleData = GetParticleData();
        if (particleData && !particleData->IsPlaying()) {
            particleData->Play();
        }
    }
}

std::unique_ptr<BaseEffectData> ParticlePlayer::CreateEffectData() {
    return std::make_unique<ParticleData>();
}

ParticleData* ParticlePlayer::GetParticleData() {
    return dynamic_cast<ParticleData*>(effectData_.get());
}

//*----------------------------- Runtime Settings -----------------------------*//

void ParticlePlayer::SetParentTransform(const WorldTransform* transform) {
    auto* particleData = GetParticleData();
    if (!particleData)
        return;

    for (auto& section : particleData->GetSectionElements()) {
        section->GetSectionParam()->SetParentTransform(transform);
    }
}

void ParticlePlayer::SetParentJoint(const Object3DAnimation* modelAnimation, const std::string& jointName) {
    auto* particleData = GetParticleData();
    if (!particleData)
        return;

    for (auto& section : particleData->GetSectionElements()) {
        section->GetSectionParam()->SetParentJoint(modelAnimation, jointName);
    }
}

void ParticlePlayer::SetFollowingPos(const Vector3* pos) {
    auto* particleData = GetParticleData();
    if (!particleData)
        return;

    for (auto& section : particleData->GetSectionElements()) {
        section->GetSectionParam()->SetFollowingPos(pos);
    }
}

void ParticlePlayer::SetParentBasePos(WorldTransform* parent) {
    auto* particleData = GetParticleData();
    if (!particleData)
        return;

    for (auto& section : particleData->GetSectionElements()) {
        section->SetParentBasePos(parent);
    }
}

void ParticlePlayer::SetTargetPosition(const Vector3& targetPos) {
    auto* particleData = GetParticleData();
    if (!particleData)
        return;

    for (auto& section : particleData->GetSectionElements()) {
        section->GetSectionParam()->SetTargetPosition(targetPos);
    }
}

//*----------------------------- Section-Specific Settings -----------------------------*//

void ParticlePlayer::SetSectionParentTransform(int32_t sectionIndex, const WorldTransform* transform) {
    auto* particleData = GetParticleData();
    if (!particleData)
        return;

    auto& sections = particleData->GetSectionElements();
    if (sectionIndex >= 0 && sectionIndex < static_cast<int32_t>(sections.size())) {
        sections[sectionIndex]->GetSectionParam()->SetParentTransform(transform);
    }
}

void ParticlePlayer::SetSectionParentJoint(int32_t sectionIndex, const Object3DAnimation* modelAnimation, const std::string& jointName) {
    auto* particleData = GetParticleData();
    if (!particleData)
        return;

    auto& sections = particleData->GetSectionElements();
    if (sectionIndex >= 0 && sectionIndex < static_cast<int32_t>(sections.size())) {
        sections[sectionIndex]->GetSectionParam()->SetParentJoint(modelAnimation, jointName);
    }
}

void ParticlePlayer::SetSectionFollowingPos(int32_t sectionIndex, const Vector3* pos) {
    auto* particleData = GetParticleData();
    if (!particleData)
        return;

    auto& sections = particleData->GetSectionElements();
    if (sectionIndex >= 0 && sectionIndex < static_cast<int32_t>(sections.size())) {
        sections[sectionIndex]->GetSectionParam()->SetFollowingPos(pos);
    }
}