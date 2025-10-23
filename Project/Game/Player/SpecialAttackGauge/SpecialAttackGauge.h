#pragma once

#include "2d/Sprite.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include <memory>

/// <summary>
/// 必殺技ゲージ管理クラス
/// </summary>
class SpecialAttackGauge {
public:
    // 初期化、更新
    void Init();
    void Update();

    void SpriteDraw(); //< スプライト描画
    void AddGauge();   //< ゲージ加算

private:
    // gauge
    float gauge_;
    float gaugeUpRate_;
    float gaugeMax_;

    // sprite
    std::unique_ptr<Sprite> gaugeFrameSprite_;
    std::unique_ptr<Sprite> gaugeSprite_;

    // position
    Vector2 gaugeFramePos_;
    Vector2 gaugePos_;

    // globalParameter
    GlobalParameter* globalParameter_;
};