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
            particleData->Init(particleName, categoryName);

            // ロード、ペアレント設定、再生
            particleData->LoadData();
            ApplyParentParametersToData(particleData);
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

void ParticlePlayer::ApplyParentParametersToData(ParticleData* particleData) {
    if (!particleData) {
        return;
    }

    // ParentTransform
    if (parentParam_.transform_) {
        particleData->SetParentTransform(parentParam_.transform_);
    }

    // ParentJoint
    if (parentParam_.modelAnimation) {
        particleData->SetParentJoint(parentParam_.modelAnimation, parentParam_.jointName);
    }

    // FollowPos
    if (parentParam_.followPos_) {
        particleData->SetFollowingPos(parentParam_.followPos_);
    }
}

// set---------------------------------------------------------------------------------------------------------------

void ParticlePlayer::SetParentTransform(const WorldTransform* transform) {
    parentParam_.transform_ = transform;

    auto* particleData = GetParticleData();
    if (particleData) {
        particleData->SetParentTransform(transform);
    }
}

void ParticlePlayer::SetParentJoint(const Object3DAnimation* modelAnimation, const std::string& jointName) {
    parentParam_.modelAnimation = modelAnimation;
    parentParam_.jointName      = jointName;

    auto* particleData = GetParticleData();
    if (particleData) {
        particleData->SetParentJoint(modelAnimation, jointName);
    }
}

void ParticlePlayer::SetFollowingPos(const Vector3* pos) {
    parentParam_.followPos_ = pos;

    auto* particleData = GetParticleData();
    if (particleData) {
        particleData->SetFollowingPos(pos);
    }
}

void ParticlePlayer::SetTargetPosition(const Vector3& targetPos) {
    auto* particleData = GetParticleData();
    if (!particleData)
        return;

    particleData->SetTargetPosition(targetPos);
}