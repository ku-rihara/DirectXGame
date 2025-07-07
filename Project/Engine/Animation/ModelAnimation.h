#pragma once

#include "3d/Model.h"
#include "3d/Object3d.h"
#include "3d/ViewProjection.h"
#include "3d/WorldTransform.h"

#include"3d/Line3d.h"

//data
#include"Animation/SkeletonData.h"
#include "AnimationData.h"
#include "struct/ModelData.h"
#include"Animation/SkinCluster.h"

#include <memory>
#include <vector>
#include <cstdint>
#include <optional>

class ModelAnimation {
public:
    Animation LoadAnimationFile(const std::string& fileName);
    Skeleton CreateSkeleton(const Node& rootNode);
    int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);
    SkinCluster CreateSkinCluster(ModelData&modelData);

    void Create(const std::string& fileName);
    void Update(const float& deltaTime);
    void Draw(const ViewProjection& viewProjection);

    void DebugDraw(const ViewProjection& viewProjection);

    Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframe, float time);
    Quaternion CalculateValueQuaternion(const std::vector<KeyframeQuaternion>& keyframe, float time);

private:
    Animation animation_;
    Skeleton skeleton_;
    Joint joint_;
    SkinCluster skinCluster_;

    WorldTransform worldTransform_;
    std::unique_ptr<Object3d> object3d_;

    Line3D line3dDrawer_;

    const std::string directoryPath_ = "Resources/Model/";
    float animationTime_;
};