#pragma once

#include "Vector3.h"
#include <cstdint>

namespace KetaEngine {

/// <summary>
/// GPUパーティクルの発生位置・発生数・発生頻度を制御するデータ
/// </summary>
struct ParticleEmit {
    Vector3 translate;
    uint32_t count;
    float frequency;
    float frequencyTime;
    uint32_t emit;
    float _padding0;
};

/// <summary>
/// 発生時のスケール・回転・座標のランダム範囲(最小/最大)を保持するデータ
/// </summary>
struct EmitTransformParams {
    Vector3 scaleMin;
    float _padding0;
    Vector3 scaleMax;
    float _padding1;

    Vector3 rotationMin;
    float _padding2;
    Vector3 rotationMax;
    float _padding3;

    Vector3 translateMin;
    float _padding4;
    Vector3 translateMax;
    float _padding5;
};

/// <summary>
/// Physics系パラメータ
/// </summary>
struct EmitPhysicsParams {
    Vector3 velocityMin;
    float _padding0;
    Vector3 velocityMax;
    float _padding1;

    Vector3 rotationSpeedMin; 
    float _padding2;
    Vector3 rotationSpeedMax;
    float _padding3;
};

/// <summary>
/// Appearance系パラメータ
/// </summary>
struct EmitAppearanceParams {
    Vector4 colorMin;
    Vector4 colorMax;

    float lifeTimeMin;
    float lifeTimeMax;

    float _padding0;
    float _padding1;
    float _padding2;
};

/// <summary>
/// フレームごとにGPUへ送る時間情報(経過時間・デルタタイム)
/// </summary>
struct PerFrame {
    float time;
    float deltaTime;
};

}; // KetaEngine
