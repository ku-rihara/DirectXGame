#pragma once

#include "3d/ViewProjection.h"

/// utility
#include "utility/ParticleEditor/ParticleEmitter.h"
#include"Effect/ImpactEffect.h"

/// std
#include <array>
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
    std::array<ParticleEffect, 3> starEffect_;

      std::list<std::unique_ptr<ImpactEffect>> effects_; /// エフェクト
public:
    PlayerEffects();

    /// ===================================================
    ///  public method
    /// ===================================================

    ///* 初期化、更新、描画
    void Init(const Vector3& position);
    void Update();
    void Draw(const ViewProjection& viewProjection);

  
    void FallEffectInit(const Vector3& pos);

    void DebriParticleEmit();
    void FallEffectUpdate();
    void StartEffectEmit();
    void ParticleUpdate(const Vector3& position);
    void RushParticleUdate(const Vector3& position);

    /// ====================================================================
    /// Editor
    /// ====================================================================

    /// ====================================================================
    /// Collision
    /// ====================================================================

    /// =========================================================================================
    /// getter
    /// =========================================================================================

    /// =========================================================================================
    /// setter
    /// =========================================================================================
};