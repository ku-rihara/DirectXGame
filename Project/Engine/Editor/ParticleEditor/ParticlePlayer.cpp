#include "ParticlePlayer.h"
#include "Frame/Frame.h"

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
    // このフレームでPlayが呼ばれなかった場合の処理
    if (isInitialized_ && !wasPlayCalledThisFrame_) {
        auto* particleData = GetParticleData();
        if (particleData && particleData->IsPlaying()) {
            // 各セクションの再生継続時間をチェック
            particleData->CheckAndPauseSectionsAfterDuration(Frame::DeltaTimeRate());
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
            AdaptParentTransform();
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

void ParticlePlayer::AdaptParentTransform() {

    auto* particleData = GetParticleData();
    if (!particleData) {
        return;
    }

    // ParentTransform
    for (auto& section : particleData->GetSectionElements()) {
        if (!parentParam_.transform_) {
            continue;
        }
        section->GetSectionParam()->SetParentTransform(parentParam_.transform_);
    }

    // ParentJoint
    for (auto& section : particleData->GetSectionElements()) {
        if (!parentParam_.modelAnimation) {
            continue;
        }
        section->GetSectionParam()->SetParentJoint(parentParam_.modelAnimation, parentParam_.jointName);
    }

    // FollowPos
    for (auto& section : particleData->GetSectionElements()) {
        if (!parentParam_.followPos_) {
            continue;
        }
        section->GetSectionParam()->SetFollowingPos(parentParam_.followPos_);
    }
}

// set---------------------------------------------------------------------------------------------------------------

void ParticlePlayer::SetParentTransform(const WorldTransform* transform) {
    parentParam_.transform_ = transform;
}
void ParticlePlayer::SetParentJoint(const Object3DAnimation* modelAnimation, const std::string& jointName) {
    parentParam_.modelAnimation = modelAnimation;
    parentParam_.jointName      = jointName;
}
void ParticlePlayer::SetFollowingPos(const Vector3* pos) {
    parentParam_.followPos_ = pos;
}

void ParticlePlayer::SetTargetPosition(const Vector3& targetPos) {
    auto* particleData = GetParticleData();
    if (!particleData)
        return;

    for (auto& section : particleData->GetSectionElements()) {
        section->GetSectionParam()->SetTargetPosition(targetPos);
    }
}
