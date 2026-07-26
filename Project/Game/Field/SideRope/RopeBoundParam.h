#pragma once

/// <summary>
/// サイドロープの伸縮・反発挙動を表すパラメータ
/// </summary>
struct RopeBoundParam {
    float maxStretch;
    float stretchResistance;
    float reboundRate;
    float delayTime;
};