#pragma once
#include"Matrix4x4.h"
#include"Vector4.h"
#include <Vector3.h>

namespace KetaEngine {

struct ShadowTransformData {
    Matrix4x4 lightCamera;
};

struct WorldMatrixData {
    Matrix4x4 world_;
};

}; // KetaEngine
