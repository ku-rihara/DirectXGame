#pragma once
#include "Vector3.h"

namespace KetaEngine {

/// <summary>
/// PBD粒子を押し出す球状の障害物(敵の当たり判定など、外部から毎フレーム渡す想定)
/// </summary>
struct PBDSphereObstacle {
    Vector3 center;
    float radius;
};

}; // namespace KetaEngine
