#pragma once

#include "3d/ViewProjection.h"
#include "Particle/CPUParticle/ParticlePlayer.h"
#include <memory>
#include <string>

/// <summary>
/// 敵のエフェクト管理クラス
/// </summary>
class EnemyEffects {
public:
    EnemyEffects()  = default;
    ~EnemyEffects() = default;

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

    /// <summary>
    /// パーティクルエフェクトの放出
    /// </summary>
    /// <param name="name">エフェクト名</param>
    void Emit(const std::string& name);

private:
    std::unique_ptr<KetaEngine::ParticlePlayer> particlePlayer_;

public:
    /// =========================================================================================
    /// getter
    /// =========================================================================================

};