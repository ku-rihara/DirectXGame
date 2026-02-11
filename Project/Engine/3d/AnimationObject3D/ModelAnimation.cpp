#include "ModelAnimation.h"

using namespace KetaEngine;
#include "3d/Model.h"
#include "Base/Descriptors/SrvManager.h"
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

SkinCluster ModelAnimation::CreateSkinCluster(ModelData& modelData, const Skeleton& skeleton, Model* model) {

    SkinCluster skinCluster = {};

    CreatePaletteResource(skinCluster, skeleton);
    CreateInputVertexSRV(skinCluster, modelData, model);
    CreateInfluenceResource(skinCluster, modelData);
    CreateOutputVertexResourceAndUAV(skinCluster, modelData);
    CreateSkinningInfoResource(skinCluster, modelData);
   

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
    for (const Node& child : node.children) {
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

///------------------------------------------------------------------------------------------------------
/// 各リソース作成
///------------------------------------------------------------------------------------------------------

void ModelAnimation::CreatePaletteResource(SkinCluster& skinCluster, const Skeleton& skeleton) {
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();
    SrvManager* srvManager  = SrvManager::GetInstance();

    skinCluster.paletteResource = dxCommon->CreateBufferResource(
        dxCommon->GetDevice(), sizeof(WellForGPU) * skeleton.joints.size());
    WellForGPU* mappedPalette = nullptr;
    skinCluster.paletteResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedPalette));
    skinCluster.mappedPalette = {mappedPalette, skeleton.joints.size()};

    uint32_t srvIndex                   = srvManager->Allocate();
    skinCluster.paletteSrvHandle.first  = srvManager->GetCPUDescriptorHandle(srvIndex);
    skinCluster.paletteSrvHandle.second = srvManager->GetGPUDescriptorHandle(srvIndex);

    srvManager->CreateForStructuredBuffer(
        srvIndex,
        skinCluster.paletteResource.Get(),
        UINT(skeleton.joints.size()),
        sizeof(WellForGPU));
}

void ModelAnimation::CreateInputVertexSRV(SkinCluster& skinCluster, ModelData& modelData, Model* model) {
    SrvManager* srvManager = SrvManager::GetInstance();

    uint32_t inputVertexSrvIndex            = srvManager->Allocate();
    skinCluster.inputVertexSrvHandle.first  = srvManager->GetCPUDescriptorHandle(inputVertexSrvIndex);
    skinCluster.inputVertexSrvHandle.second = srvManager->GetGPUDescriptorHandle(inputVertexSrvIndex);

    srvManager->CreateForStructuredBuffer(
        inputVertexSrvIndex,
        model->GetVertexResource(),
        UINT(modelData.vertices.size()),
        sizeof(VertexData));
}

void ModelAnimation::CreateInfluenceResource(SkinCluster& skinCluster, ModelData& modelData) {
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();
    SrvManager* srvManager  = SrvManager::GetInstance();

    //  Influence Resource 作成
    skinCluster.influenceResource = dxCommon->CreateBufferResource(
        dxCommon->GetDevice(),
        sizeof(VertexInfluence) * modelData.vertices.size());

    //  CPU側で初期化
    VertexInfluence* mappedInfluence = nullptr;
    skinCluster.influenceResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluence));
    std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * modelData.vertices.size());
    skinCluster.mappedInfluence = {mappedInfluence, modelData.vertices.size()};

    //  SRVを作成
    uint32_t srvIndex                     = srvManager->Allocate();
    skinCluster.influenceSrvHandle.first  = srvManager->GetCPUDescriptorHandle(srvIndex);
    skinCluster.influenceSrvHandle.second = srvManager->GetGPUDescriptorHandle(srvIndex);
    srvManager->CreateForStructuredBuffer(
        srvIndex,
        skinCluster.influenceResource.Get(),
        UINT(modelData.vertices.size()),
        sizeof(VertexInfluence));

    //  Vertex Buffer Viewを作成 
    skinCluster.influenceBufferView.BufferLocation = skinCluster.influenceResource->GetGPUVirtualAddress();
    skinCluster.influenceBufferView.SizeInBytes    = UINT(sizeof(VertexInfluence) * modelData.vertices.size());
    skinCluster.influenceBufferView.StrideInBytes  = sizeof(VertexInfluence);
}


void ModelAnimation::CreateOutputVertexResourceAndUAV(SkinCluster& skinCluster, ModelData& modelData) {
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();
    SrvManager* srvManager  = SrvManager::GetInstance();

    skinCluster.outputVertexResource = dxCommon->CreateBufferResource(
        dxCommon->GetDevice(),
        static_cast<UINT>(sizeof(VertexData) * modelData.vertices.size()),
        ViewType::UnorderedAccess);

    uint32_t outputVertexUavIndex            = srvManager->Allocate();
    skinCluster.outputVertexUavHandle.first  = srvManager->GetCPUDescriptorHandle(outputVertexUavIndex);
    skinCluster.outputVertexUavHandle.second = srvManager->GetGPUDescriptorHandle(outputVertexUavIndex);


    // UAV作成
    srvManager->CreateStructuredUAV(
        outputVertexUavIndex, skinCluster.outputVertexResource.Get(),
        UINT(modelData.vertices.size()), sizeof(VertexData));

    skinCluster.outputVertexBufferView.BufferLocation = skinCluster.outputVertexResource->GetGPUVirtualAddress();
    skinCluster.outputVertexBufferView.SizeInBytes    = static_cast<UINT>(sizeof(VertexData) * modelData.vertices.size());
    skinCluster.outputVertexBufferView.StrideInBytes  = sizeof(VertexData);
}

void ModelAnimation::CreateSkinningInfoResource(SkinCluster& skinCluster, ModelData& modelData) {
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();
    struct SkinningInformation {
        int numVertices;
    };

    skinCluster.skinningInfoResource = dxCommon->CreateBufferResource(
        dxCommon->GetDevice(), sizeof(SkinningInformation));

    SkinningInformation* mappedSkinningInfo = nullptr;
    skinCluster.skinningInfoResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedSkinningInfo));
    mappedSkinningInfo->numVertices = static_cast<int>(modelData.vertices.size());
}

