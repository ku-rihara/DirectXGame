#pragma once
#include "3d/Object3d.h"
#include "3d/ViewProjection.h"
#include "3d/WorldTransform.h"

#include "utility/ParticleEditor/ParticleEmitter.h"
#include <array>
class Field {
private:
    // モデル
    std::unique_ptr<Object3d> obj3d_ = nullptr;
    std::array<std::unique_ptr<ParticleEmitter>, 1> emitter_;

public:
    static Vector3 baseScale_;

    Field();
    ~Field();
    /// ===================================================
    /// public method
    /// ===================================================

    void Init();
    void Update();
};
