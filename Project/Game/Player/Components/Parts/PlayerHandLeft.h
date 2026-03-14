#pragma once

#include "Player/Components/Parts/BasePlayerHand.h"
#include"Particle/GPUParticle/GPUParticlePlayer.h"

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

    // 初期化、更新
    void Init() override;
    void Update() override;
    void AdjustParam() override; //< パラメータの調整
   
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