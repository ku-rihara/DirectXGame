#include "ModelAnimation.h"
#include "base/SrvManager.h"
#include "MathFunction.h"
#include "Pipeline/Object3DPiprline.h"
#include "Pipeline/SkinningObject3DPipeline.h"

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

void ModelAnimation::Create(const std::string& fileName) {
    object3d_.reset(Object3d::CreateModel(fileName));
    animations_.push_back(LoadAnimationFile(fileName));

    skeleton_ = CreateSkeleton(object3d_->GetModel()->GetModelData().rootNode);

    ModelData modelData = object3d_->GetModel()->GetModelData();
    skinCluster_        = CreateSkinCluster(modelData);

    transform_.Init();
    line3dDrawer_.Init(5120);
}

void ModelAnimation::Add(const std::string& fileName) {
    animations_.push_back(LoadAnimationFile(fileName));
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

SkinCluster ModelAnimation::CreateSkinCluster(ModelData& modelData) {

    SkinCluster skinCluster;
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();

    // palette用のResourceを確保
    skinCluster.paletteResource = dxCommon->CreateBufferResource(dxCommon->GetDevice(), sizeof(WellForGPU) * skeleton_.joints.size());
    WellForGPU* mappedPalette   = nullptr;
    skinCluster.paletteResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedPalette));
    skinCluster.mappedPalette = {mappedPalette, skeleton_.joints.size()}; // spanを使ってアクセスするように

    uint32_t srvIndex                   = SrvManager::GetInstance()->Allocate();
    skinCluster.paletteSrvHandle.first  = SrvManager::GetInstance()->GetCPUDescriptorHandle(srvIndex);
    skinCluster.paletteSrvHandle.second = SrvManager::GetInstance()->GetGPUDescriptorHandle(srvIndex);

    // palette用のSrvを作成
    SrvManager::GetInstance()->CreateSRVforStructuredBuffer(
        srvIndex, skinCluster.paletteResource.Get(), UINT(skeleton_.joints.size()), sizeof(WellForGPU));

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
    skinCluster.inverseBindPoseMatrices.resize(skeleton_.joints.size());
    std::generate(skinCluster.inverseBindPoseMatrices.begin(), skinCluster.inverseBindPoseMatrices.end(), MakeIdentity4x4);

    for (const auto& jointWeight : modelData.skinClusterData) {
        auto it = skeleton_.jointMap.find(jointWeight.first);
        if (it == skeleton_.jointMap.end()) {
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

void ModelAnimation::Update(const float& deltaTime) {
    animationTime_ += deltaTime;
    animationTime_ = std::fmod(animationTime_, animations_[currentAnimationIndex_].duration);

    if (isChange_) {

        AnimationTransition(deltaTime);
    } else {
        for (Joint& joint : skeleton_.joints) {

            // 対象のJointのAnimationがあれば、値の運用を行う
            if (auto it = animations_[currentAnimationIndex_].nodeAnimations.find(joint.name); it != animations_[currentAnimationIndex_].nodeAnimations.end()) {
                const NodeAnimation& rootNodeAnimation = (*it).second;
                joint.transform.translate              = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime_);
                joint.transform.rotate                 = CalculateValueQuaternion(rootNodeAnimation.rotate.keyframes, animationTime_);
                joint.transform.scale                  = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime_);
            }
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

    // SkinClusterの更新
    for (size_t jointIndex = 0; jointIndex < skeleton_.joints.size(); ++jointIndex) {
        assert(jointIndex < skinCluster_.inverseBindPoseMatrices.size());
        skinCluster_.mappedPalette[jointIndex].skeletonSpaceMatrix =
            skinCluster_.inverseBindPoseMatrices[jointIndex] * skeleton_.joints[jointIndex].skeletonSpaceMatrix;
        skinCluster_.mappedPalette[jointIndex].skeletonSpaceInverseTransposeMatrix =
            Inverse(Transpose(skinCluster_.mappedPalette[jointIndex].skeletonSpaceMatrix));
    }

    transform_.UpdateMatrix();
}

void ModelAnimation::AnimationTransition(const float& deltaTime) {
    // 補間タイム加算
    currentTransitionTime_ += deltaTime;
    preAnimationTime_ += deltaTime;
    currentTransitionTime_ = std::min(currentTransitionTime_, 1.0f);

    // 前のアニメーションタイム
    float preTime = std::fmod(preAnimationTime_, animations_[preAnimationIndex_].duration);

    for (Joint& joint : skeleton_.joints) {

        Vector3 toTranslate = joint.transform.translate;
        Quaternion toRotate = joint.transform.rotate;
        Vector3 toScale     = joint.transform.scale;

        // 現在のアニメーションから目標値を取得
        if (auto it = animations_[currentAnimationIndex_].nodeAnimations.find(joint.name); it != animations_[currentAnimationIndex_].nodeAnimations.end()) {
            const NodeAnimation& currentNodeAnimation = (*it).second;

            toTranslate = CalculateValue(currentNodeAnimation.translate.keyframes, animationTime_);
            toRotate    = CalculateValueQuaternion(currentNodeAnimation.rotate.keyframes, animationTime_);
            toScale     = CalculateValue(currentNodeAnimation.scale.keyframes, animationTime_);
        }

        // 前のアニメーションから開始値を取得
        if (auto it = animations_[preAnimationIndex_].nodeAnimations.find(joint.name); it != animations_[preAnimationIndex_].nodeAnimations.end()) {
            const NodeAnimation& preNodeAnimation = (*it).second;

            Vector3 fromTranslate = joint.transform.translate;
            Quaternion fromRotate = joint.transform.rotate;
            Vector3 fromScale     = joint.transform.scale;

            fromTranslate = CalculateValue(preNodeAnimation.translate.keyframes, preTime);
            fromRotate    = CalculateValueQuaternion(preNodeAnimation.rotate.keyframes, preTime);
            fromScale     = CalculateValue(preNodeAnimation.scale.keyframes, preTime);

            // 補間適用
            joint.transform.translate = Lerp(fromTranslate, toTranslate, currentTransitionTime_);
            joint.transform.scale     = Lerp(fromScale, toScale, currentTransitionTime_);
            joint.transform.rotate    = Quaternion::Slerp(fromRotate, toRotate, currentTransitionTime_);
        } 
    }

    // 補間終了判定
    if (currentTransitionTime_ >= 1.0f) {
        TransitionFinish();
    }
}

void ModelAnimation::Draw(const ViewProjection& viewProjection) {
    SkinningObject3DPipeline::GetInstance()->PreDraw(DirectXCommon::GetInstance()->GetCommandList());
    object3d_->DrawAnimation(transform_, viewProjection, skinCluster_);
    Object3DPiprline::GetInstance()->PreDraw(DirectXCommon::GetInstance()->GetCommandList());
}

void ModelAnimation::DebugDraw( const ViewProjection& viewProjection) {

    for (const Joint& joint : skeleton_.joints) {

        //
        Vector3 jointPos = TransformMatrix(transform_.GetWorldPos(), joint.skeletonSpaceMatrix);
        line3dDrawer_.DrawCubeWireframe(jointPos, Vector3(0.01f, 0.01f, 0.01f), Vector4::kWHITE());

        // Line描画
        if (joint.parent) {
            const Joint& parentJoint = skeleton_.joints[*joint.parent];
            Vector3 parentPos        = TransformMatrix(transform_.GetWorldPos(), parentJoint.skeletonSpaceMatrix);
            line3dDrawer_.SetLine(jointPos, parentPos, Vector4::kWHITE());
        }
    }
    // Joint描画
    line3dDrawer_.Draw(viewProjection);
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


 const Joint* ModelAnimation::GetJoint(const std::string& name) const {
    auto it = skeleton_.jointMap.find(name);
    if (it != skeleton_.jointMap.end()) {
        return &skeleton_.joints[it->second];
    }
    return nullptr;
}

void ModelAnimation::ChangeAnimation(const std::string& animationName) {
    // nameからIndex取得
    for (int32_t i = 0; i < animations_.size(); ++i) {

        if (animations_[i].name == animationName) {
            preAnimationIndex_     = currentAnimationIndex_;
            currentAnimationIndex_ = i;

            // 前のアニメーションの時間を保存
            preAnimationTime_ = animationTime_;

            //切り替え変数リセット
            animationTime_         = 0.0f;
            currentTransitionTime_ = 0.0f;
            isChange_              = true;

            return;
        }
    }
}

void ModelAnimation::TransitionFinish() {
    currentTransitionTime_ = 0.0f;
    isChange_              = false;
}