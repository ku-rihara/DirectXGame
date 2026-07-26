#pragma once
#include"Vector3.h"
#include"Quaternion.h"

namespace KetaEngine {

/// <summary>
/// オイラー角で回転を表すTransform構造体
/// </summary>
struct EulerTransform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

/// <summary>
/// クォータニオンで回転を表すTransform構造体
/// </summary>
struct QuaternionTransform {
    Vector3 scale;
    Quaternion rotate;
    Vector3 translate;
};

}; // KetaEngine
