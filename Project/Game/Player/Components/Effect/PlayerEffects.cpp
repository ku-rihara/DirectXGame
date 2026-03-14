#include "PlayerEffects.h"
#include "audio/Audio.h"

///=========================================================
/// 　初期化
///==========================================================
void PlayerEffects::Init(KetaEngine::WorldTransform* transform) {

    particlePlayer_.Init();
    particlePlayer_.SetFollowingPos(&transform->translation_);

   /* starSound_ = KetaEngine::Audio::GetInstance()->LoadWave("starEffect.wav");*/

  
}

///=========================================================
/// 　更新
///==========================================================
void PlayerEffects::Update(const Vector3& position) {

    particlePlayer_.SetTargetPosition(position);
    particlePlayer_.Update();

  
}

void PlayerEffects::SpecialAttackRenditionInit() {
   
}

void PlayerEffects::RushAttackEmit() {
  
}

void PlayerEffects::RushAttackRingEffectEmit() {
  
}

void PlayerEffects::FallEffectRenditionInit() {
    particlePlayer_.Play("FallEffect", "Player");
}

void PlayerEffects::Emit(const std::string& name) {
    particlePlayer_.Play(name,"Player");
}