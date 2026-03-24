#pragma once

#include "3d/AnimationObject3D/Object3DAnimation.h"
// std
#include <memory>
#include <vector>

/// <summary>
/// リザルト画面のランナーキャラクター
/// </summary>
class ResultRunner {
public:
    ResultRunner()  = default;
    ~ResultRunner() = default;

    void Init();
    void Update();

private:
    std::vector<std::unique_ptr<KetaEngine::Object3DAnimation>> runnerAnimations_;
};
