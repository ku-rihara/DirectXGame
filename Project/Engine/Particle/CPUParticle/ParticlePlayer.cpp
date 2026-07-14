#include "ParticlePlayer.h"
#include "Frame/Frame.h"

using namespace KetaEngine;

void ParticlePlayer::Init() {
    BaseEffectPlayer::Init();
    isInitialized_          = false;
    wasPlayCalledThisFrame_ = false;
    finishingEffects_.clear();
}

void ParticlePlayer::Update(float speedRate) {
    // Play関数が呼ばれていたか確認
    UpdatePlayState();

    // 優先エフェクトのガード時間を減算
    if (priorityGuardTimer_ > 0.0f) {
        priorityGuardTimer_ -= Frame::DeltaTimeRate();
        if (priorityGuardTimer_ < 0.0f) {
            priorityGuardTimer_ = 0.0f;
        }
    }

    // TargetParam ポインタが設定されていれば毎フレーム自動適用
    if (targetParam_.pos_ || targetParam_.rotate_) {
        auto* particleData = GetParticleData();
        if (particleData) {
            if (targetParam_.pos_) {
                particleData->SetTargetPosition(*targetParam_.pos_);
            }
            if (targetParam_.rotate_) {
                particleData->SetTargetRotation(*targetParam_.rotate_);
            }
        }
    }

    if (effectData_) {
        effectData_->Update(speedRate);
    }

    // 古いエフェクトを継続更新し、afterDurationで停止したら削除
    for (auto it = finishingEffects_.begin(); it != finishingEffects_.end();) {
        auto* pd = static_cast<ParticleData*>(it->get());
        if (pd) {
            pd->CheckAndPauseSectionsAfterDuration(Frame::DeltaTimeRate());
            pd->Update(speedRate);
            if (!pd->IsPlaying()) {
                it = finishingEffects_.erase(it);
                continue;
            }
        }
        ++it;
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

void ParticlePlayer::Play(const std::string& particleName, const std::string& categoryName) {
    PlayInternal(particleName, categoryName, false);
}

void ParticlePlayer::PlayPriority(const std::string& particleName, const std::string& categoryName) {
    PlayInternal(particleName, categoryName, true);
}

void ParticlePlayer::PlayInternal(const std::string& particleName, const std::string& categoryName, bool isPriority) {
    bool isSwitchingEffect = !isInitialized_ || currentCategoryName_ != categoryName || currentParticleName_ != particleName;

    // 優先エフェクト再生中は、ガード時間が残っている間、通常Emitによる差し替えを無視する
    if (isSwitchingEffect && !isPriority && currentIsPriority_ && priorityGuardTimer_ > 0.0f) {
        return;
    }

    // このフレームでPlayが呼ばれたことを記録
    wasPlayCalledThisFrame_ = true;

    // 初回呼び出し、またはパーティクル名が変わった場合のみ初期化
    if (isSwitchingEffect) {

        // 既存のエフェクトは即破棄せず、afterDurationまで継続させる
        if (effectData_) {
            finishingEffects_.push_back(std::move(effectData_));
        }
        effectData_ = CreateEffectData();

        auto* particleData = static_cast<ParticleData*>(effectData_.get());
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
        currentIsPriority_   = isPriority;
        priorityGuardTimer_  = isPriority ? kPriorityGuardDuration : 0.0f;
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
    return static_cast<ParticleData*>(effectData_.get());
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
    if (!particleData) {
        return;
    }
    particleData->SetTargetPosition(targetPos);
}

void ParticlePlayer::SetTargetRotation(const Vector3& targetRotate) {
    auto* particleData = GetParticleData();
    if (!particleData) {
        return;
    }
    particleData->SetTargetRotation(targetRotate);
}

void ParticlePlayer::SetTargetPosPtr(const Vector3* pos) {
    targetParam_.pos_ = pos;
    // 即時適用
    if (pos) {
        auto* particleData = GetParticleData();
        if (particleData) {
            particleData->SetTargetPosition(*pos);
        }
    }
}

void ParticlePlayer::SetTargetRotatePtr(const Vector3* rotate) {
    targetParam_.rotate_ = rotate;
    // 即時適用
    if (rotate) {
        auto* particleData = GetParticleData();
        if (particleData) {
            particleData->SetTargetRotation(*rotate);
        }
    }
}

void ParticlePlayer::SetGoalPosition(const Vector3& pos) {
    auto* particleData = GetParticleData();
    if (!particleData) {
        return;
    }
    particleData->SetGoalPosition(pos);
}