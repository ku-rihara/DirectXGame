#include "PlayerEffects.h"
#include"audio/Audio.h"

PlayerEffects::PlayerEffects() {}

///=========================================================
/// 　初期化
///==========================================================
void PlayerEffects::Init(const Vector3& position) {

     starSound_ = Audio::GetInstance()->LoadWave("Resources/starEffect.wav");
  
    // debri
    debriParticle_[0].emitter.reset(ParticleEmitter::CreateParticle("DebriParticle", "debri", ".obj", 100));

    // star
    starEffect_[0].emitter.reset(ParticleEmitter::CreateParticlePrimitive("StarCenterLight", PrimitiveType::Plane, 30));
    starEffect_[1].emitter.reset(ParticleEmitter::CreateParticlePrimitive("StarEffect", PrimitiveType::Plane, 30));
    starEffect_[2].emitter.reset(ParticleEmitter::CreateParticlePrimitive("StarFrame", PrimitiveType::Plane, 30));

    for (uint32_t i = 0; i < starEffect_.size(); i++) {
        starEffect_[i].emitter->SetFollowingPos(&position);
    }

    // crack
    fallCrack_.reset(ParticleEmitter::CreateParticlePrimitive("Crack", PrimitiveType::Plane, 30));

    rushParticle_[0].emitter.reset(ParticleEmitter::CreateParticlePrimitive("rushParticle", PrimitiveType::Plane, 800));
}

///=========================================================
/// 　更新
///==========================================================
void PlayerEffects::Update() {
}

///=========================================================
/// 　描画
///==========================================================
void PlayerEffects::Draw(const ViewProjection& viewProjection) {
    // 各エフェクトを更新
    effects_.reverse();
    for (std::unique_ptr<ImpactEffect>& effect : effects_) {
        if (effect) {
            effect->Draw(viewProjection);
        }
    }
    effects_.reverse();
}

void PlayerEffects::DebriParticleEmit() {
    // ガレキパーティクル
    for (uint32_t i = 0; i < debriParticle_.size(); i++) {
        debriParticle_[i].emitter->Emit();
    }
    fallCrack_->Emit();
}

void PlayerEffects::StartEffectEmit() {
    // 星パーティクル
    for (uint32_t i = 0; i < starEffect_.size(); i++) {
        starEffect_[i].emitter->Emit();
    }
      Audio::GetInstance()->PlayWave(starSound_, 0.5f);
}

void PlayerEffects::FallEffectInit(const Vector3& pos) {
    std::unique_ptr<ImpactEffect> effect = std::make_unique<ImpactEffect>();

    effect->Init(pos);
    effects_.push_back(std::move(effect));
}

void PlayerEffects::ParticleUpdate(const Vector3& position) {
    // ガレキパーティクル
    for (uint32_t i = 0; i < debriParticle_.size(); i++) {
        debriParticle_[i].emitter->SetTargetPosition(position);
        debriParticle_[i].emitter->Update();
        debriParticle_[i].emitter->EditorUpdate();
    }

    // 星パーティクル
    for (uint32_t i = 0; i < starEffect_.size(); i++) {
        starEffect_[i].emitter->Update();
    }

    fallCrack_->SetTargetPosition(Vector3(position.x, 0.0f, position.z));
    fallCrack_->Update();
}

void PlayerEffects::FallEffectUpdate() {
    // 各エフェクトを更新
    for (std::unique_ptr<ImpactEffect>& effect : effects_) {
        if (effect) {
            effect->Update();
        }
    }

    // 完了したエフェクトを消す
    effects_.erase(std::remove_if(effects_.begin(), effects_.end(), [](const std::unique_ptr<ImpactEffect>& effect) { return effect->IsFinished(); }), effects_.end());
}

void PlayerEffects::RushParticleUdate(const Vector3& position) {
    rushParticle_[0].emitter->SetTargetPosition(position);
    rushParticle_[0].emitter->Update();
    rushParticle_[0].emitter->EditorUpdate();
    rushParticle_[0].emitter->Emit();
}
