#pragma once

#include "Player/Components/Parts/BasePlayerHand.h"

/// <summary>
/// プレイヤーの右手クラス
/// </summary>
class PlayerHandRight : public BasePlayerHand {

public:
    PlayerHandRight()  = default;
    ~PlayerHandRight() = default;
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

public:
    ///===========================================
    /// getter method
    ///=============================================
  
    ///===========================================
    /// setter method
    ///=============================================

    void SetParent(KetaEngine::WorldTransform* parent) override;
};