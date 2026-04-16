#include "PlayerEffects.h"
#include "audio/Audio.h"

///=========================================================
/// 　初期化
///==========================================================
void PlayerEffects::Init(KetaEngine::WorldTransform* transform) {

    particlePlayer_.Init();
    particlePlayer_.SetParentTransform(transform);
    particlePlayer_.SetFollowingPos(&transform->translation_);
}

///=========================================================
/// 　更新
///==========================================================
void PlayerEffects::Update(const Vector3& position,const Vector3& rotation) {

    particlePlayer_.SetTargetPosition(position);
    particlePlayer_.SetTargetRotation(rotation);
    particlePlayer_.Update();
}

void PlayerEffects::FallEffectStart() {
    particlePlayer_.Play("FallEffect", "Player");
}

void PlayerEffects::DashEffectStart() {
    particlePlayer_.Play("DashEffect", "Player");
}

void PlayerEffects::Emit(const std::string& name) {
    particlePlayer_.Play(name, "Player");
}