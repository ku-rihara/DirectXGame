#pragma once
#include"Vector3.h"
#include"Vector4.h"
#include"Matrix4x4.h"
#include "Vector2.h"

namespace KetaEngine {

/// <summary>
/// GPUコンピュートシェーダーで扱う1パーティクル分のデータ
/// </summary>
struct ParticleCS {
    Vector3 translate; 
    float lifeTime; 
    Vector3 scale; 
    float currentTime; 
    Vector3 rotate;
    Vector3 rotateSpeed; 
    Vector3 velocity; 
    Vector4 color;

};

/// <summary>
/// GPUパーティクル描画に必要な視点依存データ
/// </summary>
struct PerView {
    Matrix4x4 viewProjection;
    Matrix4x4 billboardMatrix;
    int32_t billboardMode;
    int32_t alignToVelocity;
    Vector2 padding;
};

}; // KetaEngine
