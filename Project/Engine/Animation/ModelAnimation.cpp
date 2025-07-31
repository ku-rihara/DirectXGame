#include "ModelAnimation.h"
#include "base/SrvManager.h"
#include "MathFunction.h"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <Vector3.h>

#include <algorithm>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <Matrix4x4.h>
#include <struct/ModelData.h>


Skeleton ModelAnimation::CreateSkeleton(const Node& rootNode) {
    Skeleton skeleton;
    skeleton.root = CreateJoint(rootNode, {}, skeleton.joints);

    // 名前とindexのマッピングを行いアクセスしやすくする
    for (const Joint& joint : skeleton.joints) {
        skeleton.jointMap.emplace(joint.name, joint.index);
    }

    return skeleton;
}

SkinCluster ModelAnimation::CreateSkinCluster(ModelData& modelData, const Skeleton& skeleton) {

    SkinCluster skinCluster;
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();

    // palette用のResourceを確保
    skinCluster.paletteResource = dxCommon->CreateBufferResource(dxCommon->GetDevice(), sizeof(WellForGPU) * skeleton.joints.size());
    WellForGPU* mappedPalette   = nullptr;
    skinCluster.paletteResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedPalette));
    skinCluster.mappedPalette = {mappedPalette, skeleton.joints.size()}; // spanを使ってアクセスするように

    uint32_t srvIndex                   = SrvManager::GetInstance()->Allocate();
    skinCluster.paletteSrvHandle.first  = SrvManager::GetInstance()->GetCPUDescriptorHandle(srvIndex);
    skinCluster.paletteSrvHandle.second = SrvManager::GetInstance()->GetGPUDescriptorHandle(srvIndex);

    // palette用のSrvを作成
    SrvManager::GetInstance()->CreateSRVforStructuredBuffer(
        srvIndex, skinCluster.paletteResource.Get(), UINT(skeleton.joints.size()), sizeof(WellForGPU));

    // influence用のResourceを作成
    skinCluster.influenceResource    = dxCommon->CreateBufferResource(dxCommon->GetDevice(), sizeof(VertexInfluence) * modelData.vertices.size());
    VertexInfluence* mappedInfluence = nullptr;
    skinCluster.influenceResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluence));
    std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * modelData.vertices.size()); // 0埋め,weightを0にしておく
    skinCluster.mappedInfluence = {mappedInfluence, modelData.vertices.size()};

    // Influence用のVBV作
    skinCluster.influenceBufferView.BufferLocation = skinCluster.influenceResource->GetGPUVirtualAddress();
    skinCluster.influenceBufferView.SizeInBytes    = UINT(sizeof(VertexInfluence) * modelData.vertices.size());
    skinCluster.influenceBufferView.StrideInBytes  = sizeof(VertexInfluence);

    // influence用のVBVを作成
    skinCluster.inverseBindPoseMatrices.resize(skeleton.joints.size());
    std::generate(skinCluster.inverseBindPoseMatrices.begin(), skinCluster.inverseBindPoseMatrices.end(), MakeIdentity4x4);

    for (const auto& jointWeight : modelData.skinClusterData) {
        auto it = skeleton.jointMap.find(jointWeight.first);
        if (it == skeleton.jointMap.end()) {
            continue;
        }

        skinCluster.inverseBindPoseMatrices[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
        for (const auto& vertexWeight : jointWeight.second.vertexWeights) {
            auto& currentInfluence = skinCluster.mappedInfluence[vertexWeight.vertexIndex];
            for (uint32_t index = 0; index < kNumMaxInfluence; ++index) {
                if (currentInfluence.weights[index] == 0.0f) {
                    currentInfluence.weights[index]      = vertexWeight.weight;
                    currentInfluence.jointIndices[index] = (*it).second;
                    break;
                }
            }
        }
    }

    return skinCluster;
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

    animation.name = stemName;

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
            keyframe.time  = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
            keyframe.value = {-keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z}; // 右手➩左手
            nodeAnimation.translate.keyframes.push_back(keyframe);
        }

        // rotate
        for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex) {
            aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
            KeyframeQuaternion keyframe;
            keyframe.time  = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
            keyframe.value = {keyAssimp.mValue.x, -keyAssimp.mValue.y, -keyAssimp.mValue.z, keyAssimp.mValue.w}; // 右手➩左手
            nodeAnimation.rotate.keyframes.push_back(keyframe);
        }

        // scale
        for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex) {
            aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
            KeyframeVector3 keyframe;
            keyframe.time  = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
            keyframe.value = {keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z};
            nodeAnimation.scale.keyframes.push_back(keyframe);
        }
    }

    return animation;
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
