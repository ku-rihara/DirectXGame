#pragma once

#include "Player/Parts/BasePlayerHand.h"
#include"Editor/GPUParticleEditor/GPUParticlePlayer.h"

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
    void SaveAndLoad() override; //< セーブ・ロード

    /// <summary>
    /// ディゾルブ効果の適用
    /// </summary>
    /// <param name="dissolve">ディゾルブ値</param>
    void DissolveAdapt(float dissolve) override;

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