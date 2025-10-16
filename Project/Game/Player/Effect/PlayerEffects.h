#pragma once

#include "3d/ViewProjection.h"

/// utility
#include "utility/ParticleEditor/ParticleEmitter.h"

/// std
#include <array>
#include <cstdint>
#include <memory>
#include <string>

class PlayerEffects {
public:
    struct ParticleEffect {
        std::string name;
        std::unique_ptr<ParticleEmitter> emitter;
    };

public:
    PlayerEffects()  = default;
    ~PlayerEffects() = default;
    /// ===================================================
    ///  public method
    /// ===================================================

    ///* 初期化、更新、描画
    void Init(WorldTransform* transform);
    void Update(const Vector3& position);

    void SpecialAttackRenditionInit();
    void FallEffectRenditionInit();
    void RushAttackEmit();
    void RushAttackRingEffectEmit();

private:
    /// ===================================================
    /// private variaus
    /// ===================================================
  
    std::unique_ptr<ParticleEmitter> fallCrack_;
    std::array<ParticleEffect, 1> debriParticle_;
    std::array<ParticleEffect, 1> rushParticle_;
    std::array<ParticleEffect, 1> rushRingEffect_;
    std::array<ParticleEffect, 1> afterGlowEffect_;
    std::array<ParticleEffect, 3> starEffect_;

    int32_t starSound_;


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