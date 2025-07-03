#include "ModelAnimation.h"
#include "MathFunction.h"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <Vector3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <filesystem>
#include <fstream>

void ModelAnimation::Create(const std::string& fileName) {
    object3d_.reset(Object3d::CreateModel(fileName));
    animation_ = LoadAnimationFile(fileName);
    worldTransform_.Init();
    worldTransform_.rotateOder_ = RotateOder::Quaternion;

    skeleton_ = CreateSkeleton(object3d_->GetModel()->GetModelData().rootNode);

    line3dDrawer_.Init();
}

Skeleton ModelAnimation::CreateSkeleton(const Node& rootNode) {
    Skeleton skeleton;
    skeleton.root = CreateJoint(rootNode, {}, skeleton.joints);

    // 名前とindexのマッピングを行いアクセスしやすくする
    for (const Joint& joint : skeleton.joints) {
        skeleton.jointMap.emplace(joint.name, joint.index);
    }

    return skeleton;
}

int32_t ModelAnimation::CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints) {

    Joint joint;
    joint.name                = node.name;
    joint.localMatrix         = node.localMatrix;
    joint.skeletonSpaceMatrix = MakeIdentity4x4();
    joint.transform           = node.transform;
    joint.index               = int32_t(joints.size());
    joint.parent              = parent;

    joints.push_back(joint); // SkeletonのJoint列に追加
    for (const Node& child : node.cihldren) {
        // 子Jointを作成し、そのIndexを登録
        int32_t childIndex = CreateJoint(child, joint.index, joints);
        joints[joint.index].children.push_back(childIndex);
    }
    return joint.index;
}

Animation ModelAnimation::LoadAnimationFile(const std::string& fileName) {
    Animation animation;
    Assimp::Importer importer;

    std::filesystem::path path(fileName);
    std::string stemName = path.stem().string();

    std::string filePath = directoryPath_ + stemName + "/" + fileName;

    const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);
    assert(scene->mNumAnimations != 0); // アニメーションがない
    aiAnimation* animationAssimp = scene->mAnimations[0]; // 最初のアニメションをだけ採用
    animation.duration           = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond); // 時間の単位を秒に変換

    // channelを回してNodeAnimationの情報をとってくる
    for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {
        aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
        NodeAnimation& nodeAnimation    = animation.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];

        // translate
        for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {
            aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
            KeyframeVector3 keyframe;
            keyframe.time  = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond); // ここも秒に変換
            keyframe.value = {-keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z}; // 右手➩左手
            nodeAnimation.translate.keyframes.push_back(keyframe);
        }

        // rotate
        for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex) {
            aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
            KeyframeQuaternion keyframe;
            keyframe.time  = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond); // ここも秒に変換
            keyframe.value = {keyAssimp.mValue.x, -keyAssimp.mValue.y, -keyAssimp.mValue.z, keyAssimp.mValue.w}; // 右手➩左手
            nodeAnimation.rotate.keyframes.push_back(keyframe);
        }

        // scale
        for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex) {
            aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
            KeyframeVector3 keyframe;
            keyframe.time  = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond); // ここも秒に変換
            keyframe.value = {keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z};
            nodeAnimation.scale.keyframes.push_back(keyframe);
        }
    }

    return animation;
}

void ModelAnimation::Update(const float& deltaTime) {
    animationTime_ += deltaTime;

    for (Joint& joint : skeleton_.joints) {
        // 対象のJointのAnimationがあれば、値の運用を行う
        if (auto it = animation_.nodeAnimations.find(joint.name); it != animation_.nodeAnimations.end()) {
            const NodeAnimation& rootNodeAnimation = (*it).second;
            joint.transform.translate              = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime_);
            joint.transform.rotate                 = CalculateValueQuaternion(rootNodeAnimation.rotate.keyframes, animationTime_);
            joint.transform.scale                  = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime_);
        }
    }

    // 全てのJointを更新
    for (Joint& joint : skeleton_.joints) {
        joint.localMatrix = MakeAffineMatrixQuaternion(joint.transform.scale, joint.transform.rotate, joint.transform.translate);
        if (joint.parent) {
            joint.skeletonSpaceMatrix = joint.localMatrix * skeleton_.joints[*joint.parent].skeletonSpaceMatrix;
        } else {
            joint.skeletonSpaceMatrix = joint.localMatrix;
        }
    }

    /* animationTime_                   = std::fmod(animationTime_, animation_.duration);
     NodeAnimation& rootNodeAnimation = animation_.nodeAnimations[object3d_->GetModel()->GetModelData().rootNode.name];

     worldTransform_.translation_ = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime_);
     worldTransform_.quaternion_  = CalculateValueQuaternion(rootNodeAnimation.rotate.keyframes, animationTime_);
     worldTransform_.scale_       = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime_);

     worldTransform_.UpdateMatrix();*/
}

void ModelAnimation::Draw(const ViewProjection& viewProjection) {
    object3d_->Draw(worldTransform_, viewProjection);
}

void ModelAnimation::DebugDraw(const ViewProjection& viewProjection) {
    for (const Joint& joint : skeleton_.joints) {
        if (!joint.parent.has_value()) {
            continue;
        }

        const Joint& parentJoint = skeleton_.joints[*joint.parent];

        // 親と子の位置取得
        Vector3 parentPos = TransformMatrix(Vector3::UnitVector(), parentJoint.skeletonSpaceMatrix);
        Vector3 childPos  = TransformMatrix(Vector3::UnitVector(), joint.skeletonSpaceMatrix);

        // Line描画
        line3dDrawer_.SetLine(parentPos, childPos, Vector4::kWHITE());

        // Joint描画
        line3dDrawer_.DrawSphereWireframe(parentPos, 0.1f, Vector4::kWHITE());
        line3dDrawer_.DrawSphereWireframe(childPos, 0.1f, Vector4::kWHITE());

        line3dDrawer_.Draw(DirectXCommon::GetInstance()->GetCommandList(), viewProjection);
    }
}

Vector3 ModelAnimation::CalculateValue(const std::vector<KeyframeVector3>& keyframe, float time) {
    assert(!keyframe.empty());
    if (keyframe.size() == 1 || time <= keyframe[0].time) {
        return keyframe[0].value;
    }

    for (size_t i = 0; i < keyframe.size() - 1; ++i) {
        size_t nextIndex = i + 1;
        if (keyframe[i].time <= time && time <= keyframe[nextIndex].time) {
            float t = (time - keyframe[i].time) / (keyframe[nextIndex].time - keyframe[i].time);
            return Lerp(keyframe[i].value, keyframe[nextIndex].value, t);
        }
    }
    return (*keyframe.rbegin()).value;
}

Quaternion ModelAnimation::CalculateValueQuaternion(const std::vector<KeyframeQuaternion>& keyframe, float time) {
    assert(!keyframe.empty());
    if (keyframe.size() == 1 || time <= keyframe[0].time) {
        return keyframe[0].value;
    }

    for (size_t i = 0; i < keyframe.size() - 1; ++i) {
        size_t nextIndex = i + 1;
        if (keyframe[i].time <= time && time <= keyframe[nextIndex].time) {
            float t = (time - keyframe[i].time) / (keyframe[nextIndex].time - keyframe[i].time);
            return Quaternion::Slerp(keyframe[i].value, keyframe[nextIndex].value, t);
        }
    }
    return (*keyframe.rbegin()).value;
}