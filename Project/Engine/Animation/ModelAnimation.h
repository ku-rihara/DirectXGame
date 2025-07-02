#pragma once

#include "3d/Model.h"
#include "3d/Object3d.h"
#include "3d/ViewProjection.h"
#include "3d/WorldTransform.h"
#include "AnimationData.h"
#include "struct/ModelData.h"
#include <memory>
#include <vector>

class ModelAnimation {
public:
    Animation LoadAnimationFile(const std::string& fileName);

    void Create(const std::string& fileName);
    void Update(const float& deltaTime);
    void Draw(const ViewProjection& viewProjection);

    Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframe, float time);
    Quaternion CalculateValueQuaternion(const std::vector<KeyframeQuaternion>& keyframe, float time);

private:
    Animation animation_;
    WorldTransform worldTransform_;
    std::unique_ptr<Object3d> object3d_;

    const std::string directoryPath_ = "Resources/Model/";
    float animationTime_;
};