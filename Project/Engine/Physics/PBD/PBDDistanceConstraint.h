#pragma once
#include <cstdint>

namespace KetaEngine {

/// <summary>
/// 2粒子間の距離を一定に保つ制約
/// </summary>
struct PBDDistanceConstraint {
    uint32_t particleA;
    uint32_t particleB;
    float restLength;
    float lambda = 0.0f; 
};

}; // namespace KetaEngine
