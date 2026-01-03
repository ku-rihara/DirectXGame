#include "EnemyEffects.h"

void EnemyEffects::Init(KetaEngine::WorldTransform* transform) {
    particlePlayer_ = std::make_unique<KetaEngine::ParticlePlayer>();
    particlePlayer_->Init();
    particlePlayer_->SetFollowingPos(&transform->translation_);
}

void EnemyEffects::Update(const Vector3& position) {
    particlePlayer_->SetTargetPosition(position);
    particlePlayer_->Update();

}

void EnemyEffects::Emit(const std::string& name) {
    particlePlayer_->Play("Enemy", name);
}