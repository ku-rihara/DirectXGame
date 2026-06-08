#include "PlayerEffects.h"
#include "audio/Audio.h"

///=========================================================
/// 　初期化
///==========================================================
void PlayerEffects::Init(KetaEngine::WorldTransform* transform) {

    // worldTransformのポインタを取得
    pParentTransform_ = transform;

    // パーティクルプレイヤー初期化
    for (auto& player : particlePlayers_) {
        player = std::make_unique<KetaEngine::ParticlePlayer>();
        player->Init();
        player->SetParentTransform(transform);
        player->SetFollowingPos(&transform->translation_);
    }
}

///=========================================================
/// 　更新
///==========================================================
void PlayerEffects::Update(const Vector3& position, const Vector3& rotation) {
    for (auto& player : particlePlayers_) {
        player->SetTargetPosition(position);
        player->SetTargetRotation(rotation);
        player->Update();
    }
}

void PlayerEffects::FallEffectStart() {
    Emit("FallEffect");
}

void PlayerEffects::DashEffectStart() {
    Emit("DashEffect");
}

void PlayerEffects::Emit(const std::string& name) {
    // 空いているプレイヤーを探す
    for (auto& player : particlePlayers_) {
        if (!player->IsPlaying()) {
            player->Play(name, "Player");
            return;
        }
    }

    // 全て埋まっていたら、最初のスロットで強制再生
    particlePlayers_[0]->Play(name, "Player");
}

bool PlayerEffects::IsAnyParticlePlaying() const {

    // 再生中のプレイヤーが1つでもあればtrueを返す
    for (const auto& player : particlePlayers_) {
        if (player->IsPlaying()) {
            return true;
        }
    }
    return false;
}