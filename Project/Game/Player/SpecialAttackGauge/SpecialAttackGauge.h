#pragma once

#include "2d/Sprite.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include <memory>

class SpecialAttackGauge {
public:

    //　init,update,draw
    void Init();
    void Update();
    void SpriteDraw();
 
    //加算システム
    void AddGauge();

private:

    //gauge
    float gauge_;
    float gaugeUpRate_;
    float gaugeMax_;

    // sprite
    std::unique_ptr<Sprite> gaugeFrameSprite_;
    std::unique_ptr<Sprite> gaugeSprite_;

    //position
    Vector2 gaugeFramePos_;
    Vector2 gaugePos_;

    // glovalParameter
    GlobalParameter* globalParameter_;

};