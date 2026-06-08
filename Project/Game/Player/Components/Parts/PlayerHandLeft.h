#pragma once

#include "Particle/GPUParticle/GPUParticlePlayer.h"
#include "Player/Components/Parts/BasePlayerHand.h"

/// <summary>
/// プレイヤーの左手クラス
/// </summary>
class PlayerHandLeft : public BasePlayerHand {
public:
    PlayerHandLeft()  = default;
    ~PlayerHandLeft() = default;
    ///===========================================
    /// public method
    ///=============================================

    // 初期化、更新、パラメータ調整
    void Init() override;
    void Update(bool isDeath) override;
    void AdjustParam() override;
private:
    ///===========================================
    /// private variant
    ///=============================================

    KetaEngine::GPUParticlePlayer gpuParticlePlayer_;

public:
    ///===========================================
    /// getter method
    ///=============================================

    ///===========================================
    /// setter method
    ///=============================================

    void SetParent(KetaEngine::WorldTransform* parent) override;
};