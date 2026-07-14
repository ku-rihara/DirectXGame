#include "EnemyEffects.h"

void EnemyEffects::Init(KetaEngine::WorldTransform* transform) {
    particlePlayer_ = std::make_unique<KetaEngine::ParticlePlayer>();
    particlePlayer_->Init();
    particlePlayer_->SetFollowingPos(&transform->translation_);
}

void EnemyEffects::Update(const Vector3& position) {
    particlePlayer_->SetTargetPosition(position);
    particlePlayer_->SetGoalPosition(goalPos_);
    particlePlayer_->Update();
}

void EnemyEffects::Emit(const std::string& name) {
    particlePlayer_->Play(name, "Enemy");
}

void EnemyEffects::EmitPriority(const std::string& name) {
    particlePlayer_->PlayPriority(name, "Enemy");
}

void EnemyEffects::SetTargetObjectPos(const Vector3& pos) {
    goalPos_ = pos;
}