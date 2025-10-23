#pragma once

#include "3d/ViewProjection.h"

/// utility
#include "utility/ParticleEditor/ParticleEmitter.h"

/// std
#include <array>
#include <cstdint>
#include <memory>
#include <string>

/// <summary>
/// プレイヤーのエフェクト管理クラス
/// </summary>
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

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="transform">ワールド変換データ</param>
    void Init(WorldTransform* transform);

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="position">位置座標</param>
    void Update(const Vector3& position);

    void SpecialAttackRenditionInit(); //< 必殺技演出の初期化
    void FallEffectRenditionInit(); //< 落下エフェクト演出の初期化
    void RushAttackEmit(); //< ラッシュ攻撃エフェクトの放出
    void RushAttackRingEffectEmit(); //< ラッシュ攻撃リングエフェクトの放出

private:
    /// ===================================================
    /// private variables
    /// ===================================================

    std::unique_ptr<ParticleEmitter> fallCrack_;
    std::array<ParticleEffect, 1> debriParticle_;
    std::array<ParticleEffect, 1> rushParticle_;
    std::array<ParticleEffect, 1> rushRingEffect_;
    std::array<ParticleEffect, 1> afterGlowEffect_;
    std::array<ParticleEffect, 3> starEffect_;

    int32_t starSound_;

public:
    /// =========================================================================================
    /// getter
    /// =========================================================================================

    /// =========================================================================================
    /// setter
    /// =========================================================================================
};