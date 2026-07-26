#pragma once
#include"Matrix4x4.h"

namespace KetaEngine {

/// <summary>
/// シャドウマップ描画用のライト視点行列データ
/// </summary>
struct ShadowTransformData {
    Matrix4x4 lightCamera;
};

/// <summary>
/// シャドウマップ描画対象のワールド行列データ
/// </summary>
struct WorldMatrixData {
    Matrix4x4 world_;
};

}; // KetaEngine
