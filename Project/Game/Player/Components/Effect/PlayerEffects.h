#pragma once

#include "3d/ViewProjection.h"

/// utility
#include "Particle/CPUParticle/ParticlePlayer.h"

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
    PlayerEffects()  = default;
    ~PlayerEffects() = default;
    /// ===================================================
    ///  public method
    /// ===================================================

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="transform">ワールド変換データ</param>
    void Init(KetaEngine::WorldTransform* transform);

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="position">位置座標</param>
    void Update(const Vector3& position);

    void Emit(const std::string&name);

    // 落下エフェクト開始処理
    void FallEffectStart(); 
    // ダッシュエフェクト開始処理
    void DashEffectStart();

private:
    /// ===================================================
    /// private variables
    /// ===================================================

   

    KetaEngine::ParticlePlayer particlePlayer_;

public:
    /// =========================================================================================
    /// getter
    /// =========================================================================================

    /// =========================================================================================
    /// setter
    /// =========================================================================================
};