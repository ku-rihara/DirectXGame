#pragma once
#include "Vector3.h"

namespace KetaEngine {

/// <summary>
/// PBDの1粒子分のデータ
/// </summary>
struct PBDParticle {
    Vector3 position;          // 現在位置
    Vector3 predictedPosition; // 制約解決前の予測位置
    Vector3 velocity;          // 速度
    float invMass = 1.0f;      // 質量の逆数
};

}; // namespace KetaEngine
