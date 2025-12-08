#include "ParticlePlayer.h"

void ParticlePlayer::Init() {
    BaseEffectPlayer::Init();
}

void ParticlePlayer::Update(float speedRate) {
    if (effectData_) {
        effectData_->Update(speedRate);
    }
}

void ParticlePlayer::Play(const std::string& particleName) {
    if (effectData_) {
        effectData_->Pause();
    }

    effectData_.reset();
    effectData_ = CreateEffectData();

    auto* particleData = dynamic_cast<ParticleData*>(effectData_.get());
    if (particleData) {
        particleData->Init(particleName);
        particleData->LoadData();
        particleData->Play();
    }

    currentEffectName_ = particleName;
    currentCategoryName_.clear();
}

void ParticlePlayer::PlayInCategory(const std::string& categoryName, const std::string& particleName) {
    if (effectData_) {
        effectData_->Pause();
    }

    effectData_.reset();
    effectData_ = CreateEffectData();

    auto* particleData = dynamic_cast<ParticleData*>(effectData_.get());
    if (particleData) {
        particleData->InitWithCategory(particleName, categoryName);
        particleData->LoadData();
        particleData->Play();
    }

    currentCategoryName_ = categoryName;
    currentEffectName_   = particleName;
}

void ParticlePlayer::Draw() {
    auto* particleData = dynamic_cast<ParticleData*>(effectData_.get());
    if (particleData) {
        particleData->Draw();
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
        section->GetParameters().parentTransform = transform;
    }
}

void ParticlePlayer::SetParentJoint(const Object3DAnimation* modelAnimation, const std::string& jointName) {
    auto* particleData = GetParticleData();
    if (!particleData)
        return;

    for (auto& section : particleData->GetSectionElements()) {
        section->GetParameters().jointParent.animation = modelAnimation;
        section->GetParameters().jointParent.name      = jointName;
    }
}

void ParticlePlayer::SetFollowingPos(const Vector3* pos) {
    auto* particleData = GetParticleData();
    if (!particleData)
        return;

    for (auto& section : particleData->GetSectionElements()) {
        section->GetParameters().followingPos_ = pos;
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
        section->GetParameters().targetPos = targetPos;
    }
}

//*----------------------------- Section-Specific Settings -----------------------------*//

void ParticlePlayer::SetSectionParentTransform(int32_t sectionIndex, const WorldTransform* transform) {
    auto* particleData = GetParticleData();
    if (!particleData)
        return;

    auto& sections = particleData->GetSectionElements();
    if (sectionIndex >= 0 && sectionIndex < static_cast<int32_t>(sections.size())) {
        sections[sectionIndex]->GetParameters().parentTransform = transform;
    }
}

void ParticlePlayer::SetSectionParentJoint(int32_t sectionIndex, const Object3DAnimation* modelAnimation, const std::string& jointName) {
    auto* particleData = GetParticleData();
    if (!particleData)
        return;

    auto& sections = particleData->GetSectionElements();
    if (sectionIndex >= 0 && sectionIndex < static_cast<int32_t>(sections.size())) {
        sections[sectionIndex]->GetParameters().jointParent.animation = modelAnimation;
        sections[sectionIndex]->GetParameters().jointParent.name      = jointName;
    }
}

void ParticlePlayer::SetSectionFollowingPos(int32_t sectionIndex, const Vector3* pos) {
    auto* particleData = GetParticleData();
    if (!particleData)
        return;

    auto& sections = particleData->GetSectionElements();
    if (sectionIndex >= 0 && sectionIndex < static_cast<int32_t>(sections.size())) {
        sections[sectionIndex]->GetParameters().followingPos_ = pos;
    }
}