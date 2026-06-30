#include "PlayerEffects.h"
#include "Frame/Frame.h"

void PlayerEffects::Init(KetaEngine::WorldTransform* transform) {

    pParentTransform_ = transform;

    for (auto& player : particlePlayers_) {
        player.Init();
        player.SetParentTransform(transform);
        player.SetFollowingPos(&transform->translation_);
    }

    timeScalePlayer_.Init();
    postEffectPlayer_.Init();
}

void PlayerEffects::Update(const Vector3& position, const Vector3& rotation) {
    for (auto& player : particlePlayers_) {
        player.SetTargetPosition(position);
        player.SetTargetRotation(rotation);
        player.Update();
    }
    timeScalePlayer_.Update(KetaEngine::Frame::DeltaTime());
    postEffectPlayer_.Update();
}

void PlayerEffects::PlayHitStop(const std::string& timeScaleName) {
    timeScalePlayer_.Play(timeScaleName);
}

void PlayerEffects::PlayPostEffect(const std::string& effectName) {
    postEffectPlayer_.Play(effectName, "Player");
}

void PlayerEffects::FallEffectStart() {
    Emit("FallEffect");
}

void PlayerEffects::DashEffectStart() {
    Emit("DashEffect");
}

void PlayerEffects::Emit(const std::string& name) {
    for (auto& player : particlePlayers_) {
        if (!player.IsPlaying()) {
            player.Play(name, "Player");
            return;
        }
    }
    particlePlayers_[0].Play(name, "Player");
}

bool PlayerEffects::IsAnyParticlePlaying() const {
    for (const auto& player : particlePlayers_) {
        if (player.IsPlaying()) {
            return true;
        }
    }
    return false;
}


void PlayerEffects::ResetHitStop() {
    timeScalePlayer_.Reset();
}