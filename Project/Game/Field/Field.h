#pragma once
#include "3D/Object3d/Object3d.h"
#include "3D/ViewProjection.h"
#include "3D/WorldTransform.h"
#include "Particle/GPUParticle/GPUParticlePlayer.h"

/// <summary>
/// ゲームフィールドクラス
/// </summary>
class Field {
public:
    static Vector3 baseScale_;

    Field()  = default;
    ~Field() = default;

    // 初期化、更新
    void Init();
    void Update();

private:
    std::unique_ptr<KetaEngine::Object3d> obj3d_ = nullptr;
    KetaEngine::GPUParticlePlayer gpuParticlePlayer_;
};