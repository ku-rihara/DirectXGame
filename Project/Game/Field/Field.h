#pragma once
#include "3d/Object3d.h"
#include "3d/ViewProjection.h"
#include "3d/WorldTransform.h"
#include "utility/ParticleEditor/ParticleEmitter.h"
#include <array>

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
    std::unique_ptr<Object3d> obj3d_ = nullptr;
    std::array<std::unique_ptr<ParticleEmitter>, 1> emitter_;
};