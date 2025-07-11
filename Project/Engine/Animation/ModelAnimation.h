#pragma once

#include "3d/Model.h"
#include "3d/Object3d.h"
#include "3d/ViewProjection.h"
#include "3d/WorldTransform.h"

#include "3d/Line3d.h"

// data
#include "Animation/SkeletonData.h"
#include "Animation/SkinCluster.h"
#include "AnimationData.h"
#include "struct/ModelData.h"

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

class ModelAnimation {
public:
    ModelAnimation()  = default;
    ~ModelAnimation() = default;

    // load
    Animation LoadAnimationFile(const std::string& fileName);
    Skeleton CreateSkeleton(const Node& rootNode);
    int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);
    SkinCluster CreateSkinCluster(ModelData& modelData);

    // create,add
    void Create(const std::string& fileName);
    void Add(const std::string& fileName);
    void ChangeAnimation(const std::string& animationName);

    // update,draw
    void Update(const float& deltaTime);
    void Draw(const ViewProjection& viewProjection);
    void DebugDraw(const ViewProjection& viewProjection);

    // calculate
    Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframe, float time);
    Quaternion CalculateValueQuaternion(const std::vector<KeyframeQuaternion>& keyframe, float time);

private:
    void TransitionFinish();
    void AnimationTransition(const float& deltaTime);

public:
    WorldTransform transform_;

private:
    std::unique_ptr<Object3d> object3d_;
    std::vector<Animation> animations_;
    Skeleton skeleton_;
    Joint joint_;
    SkinCluster skinCluster_;
    Line3D line3dDrawer_;

    const std::string directoryPath_ = "Resources/Model/";
    float animationTime_;

    int32_t preAnimationIndex_     = 0;
    int32_t currentAnimationIndex_ = 0;
    float currentTransitionTime_   = 0.0f;
    bool isChange_                 = false;

public:
    const WorldTransform& GetWorldTransform() const { return transform_; }
    const Skeleton& GetSkeleton() const { return skeleton_; }
    const Joint* GetJoint(const std::string& name) const;
};