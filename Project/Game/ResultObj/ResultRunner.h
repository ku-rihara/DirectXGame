#pragma once

#include"3d/AnimationObject3D/Object3DAnimation.h"
// std
#include<vector>
#include <memory>

class ResultRunner {
public:
    ResultRunner()  = default;
    ~ResultRunner() = default;

private:

    std::vector<std::unique_ptr<KetaEngine::Object3DAnimation>> runnerAnimations_;

public:
};