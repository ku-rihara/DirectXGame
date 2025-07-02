#include "ModelAnimation.h"
#include "MathFunction.h"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <Vector3.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <fstream>
#include <filesystem>


void ModelAnimation::Create(const std::string& fileName) {
    object3d_.reset(Object3d::CreateModel(fileName));
    animation_ = LoadAnimationFile(fileName);
    worldTransform_.Init();
    worldTransform_.rotateOder_ = RotateOder::Quaternion;
}


Animation ModelAnimation::LoadAnimationFile(const std::string& fileName) {
    Animation animation;
    Assimp::Importer importer;

      std::filesystem::path path(fileName);
    std::string stemName  = path.stem().string();

    std::string filePath = directoryPath_ + stemName +"/" + fileName;

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

    animationTime_                   = std::fmod(animationTime_, animation_.duration);
    NodeAnimation& rootNodeAnimation = animation_.nodeAnimations[object3d_->GetModel()->GetModelData().rootNode.name];

    worldTransform_.translation_ = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime_);
    worldTransform_.quaternion_  = CalculateValueQuaternion(rootNodeAnimation.rotate.keyframes, animationTime_);
    worldTransform_.scale_       = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime_);

    worldTransform_.UpdateMatrix();
}


void ModelAnimation::Draw(const ViewProjection& viewProjection) {
    object3d_->Draw(worldTransform_, viewProjection);
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