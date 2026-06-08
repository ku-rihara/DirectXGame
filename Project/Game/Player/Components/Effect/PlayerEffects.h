#pragma once

// 3d
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
    void Update(const Vector3& position, const Vector3& rotation);

    /// <summary>
    /// エミット
    /// </summary>
    /// <param name="name">エフェクト名</param>
    void Emit(const std::string&name);

    // 落下エフェクト開始処理
    void FallEffectStart(); 
    // ダッシュエフェクト開始処理
    void DashEffectStart();

private:
    /// ===================================================
    /// private variables
    /// ===================================================

    // 複数の種類のエフェクトを同時に再生できるよう配列で管理
    static constexpr size_t kMaxParticles = 8;
    std::array<std::unique_ptr<KetaEngine::ParticlePlayer>, kMaxParticles> particlePlayers_;

    // PearentTransformのポインタ
    KetaEngine::WorldTransform* pParentTransform_ = nullptr;

public:
    /// =========================================================================================
    /// getter
    /// =========================================================================================
    bool IsAnyParticlePlaying() const;

    /// =========================================================================================
    /// setter
    /// =========================================================================================
};