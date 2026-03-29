#pragma once

#include "Vector4.h"
#include <array>
#include <cstdint>

struct KillBonusColorTier {
    int32_t threshold = 2;
    Vector4 color     = {1.0f, 1.0f, 1.0f, 1.0f};
};

/// ティア配列からkillCount/comboCountに対応する色を取得
inline Vector4 GetKillBonusTierColor(int32_t count, const std::array<KillBonusColorTier, 3>& tiers) {
    Vector4 result = tiers[0].color;
    for (const auto& tier : tiers) {
        if (count >= tier.threshold) {
            result = tier.color;
        }
    }
    return result;
}

// 後方互換エイリアス
using KillBonusSimKillColorTier = KillBonusColorTier;
