#pragma once

#include "3d/ViewProjection.h"

/// utility
#include "Effect/ImpactEffect.h"
#include "utility/ParticleEditor/ParticleEmitter.h"

/// std
#include <array>
#include <cstdint>
#include <memory>
#include <string>

class PlayerEffects {

    struct ParticleEffect {
        std::string name;
        std::unique_ptr<ParticleEmitter> emitter;
    };

private:
    /// ===================================================
    /// private variaus
    /// ===================================================
  
    std::unique_ptr<ParticleEmitter> fallCrack_;
    std::array<ParticleEffect, 1> debriParticle_;
    std::array<ParticleEffect, 1> rushParticle_;
    std::array<ParticleEffect, 1> rushRingEffect_;
    std::array<ParticleEffect, 3> starEffect_;

    std::list<std::unique_ptr<ImpactEffect>> effects_; /// エフェクト

    int32_t starSound_;

public:
    PlayerEffects()=default;
    ~PlayerEffects() = default;
    /// ===================================================
    ///  public method
    /// ===================================================

    ///* 初期化、更新、描画
    void Init(const Vector3& position);
    void Update(const Vector3& position);
    void Draw(const ViewProjection& viewProjection);

    void SpecialAttackRenditionInit();
    void FallEffectRenditionInit(const Vector3& pos);
    void RushAttackEmit();
    void RushAttackRingEffectEmit();

    /// ====================================================================
    /// Editor
    /// ====================================================================

    /// =========================================================================================
    /// getter
    /// =========================================================================================

    /// =========================================================================================
    /// setter
    /// =========================================================================================
};