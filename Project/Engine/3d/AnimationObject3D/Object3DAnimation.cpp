#include "Object3DAnimation.h"

using namespace KetaEngine;
// 3D
#include "3d/ModelManager.h"
// Animation
#include "AnimationRegistry.h"
// Base
#include "Base/Dx/DirectXCommon.h"
// math
#include "MathFunction.h"
// Pipeline
#include "Pipeline/CSPipelineManager.h"
// std
#include <algorithm>
#include <cassert>

Object3DAnimation::~Object3DAnimation() {
    if (AnimationRegistry::GetInstance()) {
        AnimationRegistry::GetInstance()->UnregisterAnimation(this);
    }
}

Object3DAnimation* Object3DAnimation::CreateModel(const std::string& fileName) {
    std::unique_ptr<Object3DAnimation> animationObj = std::make_unique<Object3DAnimation>();
    animationObj->Create(fileName);

    // AnimationRegistryに登録
    AnimationRegistry::GetInstance()->RegisterAnimation(animationObj.get());

    return animationObj.release();
}

///============================================================
/// 作成
///============================================================
void Object3DAnimation::Create(const std::string& fileName) {
    // ModelAnimationを読み込み専用として使用
    modelAnimation_ = std::make_unique<ModelAnimation>();

    // Object3Dの作成とモデル設定
    ModelManager::GetInstance()->LoadModel(fileName);
    SetModelByName(fileName);

    // アニメーションデータの読み込み
    animations_.push_back(modelAnimation_->LoadAnimationFile(fileName));

    // スケルトンの作成
    if (model_) {
        skeleton_ = modelAnimation_->CreateSkeleton(model_->GetModelData().rootNode);

        // スキンクラスターの作成
        ModelData modelData = model_->GetModelData();
        skinCluster_        = modelAnimation_->CreateSkinCluster(modelData, skeleton_, model_);
    }

    // リソース作成
    CreateWVPResource();
    CreateMaterialResource();
    CreateShadowMap();

    // 初期化
    Init();
}

///============================================================
/// 初期化
///============================================================
void Object3DAnimation::Init() {
    transform_.Init();
    line3dDrawer_.reset(Line3D::Create(5120));
}

///============================================================
/// アニメーション追加
///============================================================
void Object3DAnimation::Add(const std::string& fileName) {
    if (modelAnimation_) {
        animations_.push_back(modelAnimation_->LoadAnimationFile(fileName));
    }
}

///============================================================
/// アニメーション変更
///============================================================
void Object3DAnimation::ChangeAnimation(const std::string& animationName) {
    // nameからIndex取得
    for (int32_t i = 0; i < animations_.size(); ++i) {
        if (animations_[i].name == animationName) {
            preAnimationIndex_     = currentAnimationIndex_;
            currentAnimationIndex_ = i;

            // 前のアニメーションの時間を保存
            preAnimationTime_ = animationTime_;

            // 前のアニメーションが終了状態だったかを記録
            float preDuration        = animations_[preAnimationIndex_].duration;
            wasPreAnimationFinished_ = (animationTime_ >= preDuration);

            // 切り替え変数リセット
            animationTime_         = 0.0f;
            currentTransitionTime_ = 0.0f;
            isChange_              = true;
            hasLoopedThisFrame_    = false;

            return;
        }
    }
}

///============================================================
/// 更新
///============================================================
void Object3DAnimation::Update(float deltaTime) {
    if (animations_.empty()) {
        return;
    }

    UpdateAnimation(deltaTime);
    UpdateSkeleton();
    UpdateSkinCluster();

    // Transform更新
    transform_.UpdateMatrix();
}

///============================================================
/// アニメーション更新
///============================================================
void Object3DAnimation::UpdateAnimation(float deltaTime) {
    float prevTime = animationTime_;
    animationTime_ += deltaTime;

    float duration      = animations_[currentAnimationIndex_].duration;
    hasLoopedThisFrame_ = false;

    // ループ処理
    if (animationTime_ >= duration) {
        if (isLoop_) {
            // ループする場合
            animationTime_      = std::fmod(animationTime_, duration);
            hasLoopedThisFrame_ = true;

            // 現在のアニメーションに対応するコールバック実行
            const std::string& currentAnimName = animations_[currentAnimationIndex_].name;
            auto it                            = animationEndCallbacks_.find(currentAnimName);
            if (it != animationEndCallbacks_.end() && it->second) {
                it->second();
            }
        } else {
            // ループしない場合、最後のフレームで停止
            if (prevTime < duration) {
                // 初めて終端に到達した時のみコールバック実行
                animationTime_      = duration;
                hasLoopedThisFrame_ = true;

                // 現在のアニメーションに対応するコールバック実行
                const std::string& currentAnimName = animations_[currentAnimationIndex_].name;
                auto it                            = animationEndCallbacks_.find(currentAnimName);
                if (it != animationEndCallbacks_.end() && it->second) {
                    it->second();
                }
            } else {
                // すでに終端にいる場合はそのまま
                animationTime_ = duration;
            }
        }
    }

    if (isChange_) {
        AnimationTransition(deltaTime);
    } else {
        // 通常のアニメーション更新
        for (Joint& joint : skeleton_.joints) {
            // 対象のJointのAnimationがあれば、値の適用を行う
            if (auto it = animations_[currentAnimationIndex_].nodeAnimations.find(joint.name);
                it != animations_[currentAnimationIndex_].nodeAnimations.end()) {

                const NodeAnimation& nodeAnimation = (*it).second;
                joint.transform.translate          = CalculateValue(nodeAnimation.translate.keyframes, animationTime_);
                joint.transform.rotate             = CalculateValueQuaternion(nodeAnimation.rotate.keyframes, animationTime_);
                joint.transform.scale              = CalculateValue(nodeAnimation.scale.keyframes, animationTime_);
            }
        }
    }
}

///============================================================
/// スケルトン更新
///============================================================
void Object3DAnimation::UpdateSkeleton() {
    // 全てのJointを更新
    for (Joint& joint : skeleton_.joints) {
        joint.localMatrix = MakeAffineMatrixQuaternion(
            joint.transform.scale,
            joint.transform.rotate,
            joint.transform.translate);

        // skeletonSpaceMatrixの計算
        if (joint.parent) {
            joint.skeletonSpaceMatrix = joint.localMatrix * skeleton_.joints[*joint.parent].skeletonSpaceMatrix;
        } else {
            joint.skeletonSpaceMatrix = joint.localMatrix;
        }
    }
}

///============================================================
/// スキンクラスター更新
///============================================================
void Object3DAnimation::UpdateSkinCluster() {
    // SkinClusterの更新
    for (size_t jointIndex = 0; jointIndex < skeleton_.joints.size(); ++jointIndex) {
        assert(jointIndex < skinCluster_.inverseBindPoseMatrices.size());

        /// スキニング行列の計算
        Matrix4x4 skinMatrix =
            skinCluster_.inverseBindPoseMatrices[jointIndex] * skeleton_.joints[jointIndex].skeletonSpaceMatrix;

        // GPUメモリに書き込む
        skinCluster_.mappedPalette[jointIndex].skeletonSpaceMatrix                 = skinMatrix;
        skinCluster_.mappedPalette[jointIndex].skeletonSpaceInverseTransposeMatrix = Inverse(Transpose(skinMatrix));
    }
}

///============================================================
/// アニメーション遷移
///============================================================
void Object3DAnimation::AnimationTransition(float deltaTime) {
    // 補間タイム加算
    currentTransitionTime_ += deltaTime / transitionDuration_;
    currentTransitionTime_ = std::min(currentTransitionTime_, 1.0f);

    float preDuration = animations_[preAnimationIndex_].duration;
    float preTime;

    // 前のアニメーションが終了状態だった場合は最終フレームを維持
    if (wasPreAnimationFinished_) {
        preTime = preDuration;
    } else {
        // まだ終わっていない場合は時間を進める
        preAnimationTime_ += deltaTime;
        preTime = std::fmod(preAnimationTime_, preDuration);
    }

    for (Joint& joint : skeleton_.joints) {
        Vector3 toTranslate = joint.transform.translate;
        Quaternion toRotate = joint.transform.rotate;
        Vector3 toScale     = joint.transform.scale;

        // 現在のアニメーションから目標値を取得
        if (auto it = animations_[currentAnimationIndex_].nodeAnimations.find(joint.name);
            it != animations_[currentAnimationIndex_].nodeAnimations.end()) {

            const NodeAnimation& currentNodeAnimation = (*it).second;
            toTranslate                               = CalculateValue(currentNodeAnimation.translate.keyframes, animationTime_);
            toRotate                                  = CalculateValueQuaternion(currentNodeAnimation.rotate.keyframes, animationTime_);
            toScale                                   = CalculateValue(currentNodeAnimation.scale.keyframes, animationTime_);
        }

        // 前のアニメーションから開始値を取得
        if (auto it = animations_[preAnimationIndex_].nodeAnimations.find(joint.name);
            it != animations_[preAnimationIndex_].nodeAnimations.end()) {

            const NodeAnimation& preNodeAnimation = (*it).second;
            Vector3 fromTranslate                 = CalculateValue(preNodeAnimation.translate.keyframes, preTime);
            Quaternion fromRotate                 = CalculateValueQuaternion(preNodeAnimation.rotate.keyframes, preTime);
            Vector3 fromScale                     = CalculateValue(preNodeAnimation.scale.keyframes, preTime);

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

///============================================================
/// 遷移終了
///============================================================
void Object3DAnimation::TransitionFinish() {
    currentTransitionTime_   = 0.0f;
    isChange_                = false;
    wasPreAnimationFinished_ = false;
}

void Object3DAnimation::CSSkinning() {
    auto commandList = DirectXCommon::GetInstance()->GetCommandList();

    // Compute Shader用リソース設定
    commandList->SetComputeRootDescriptorTable(0, skinCluster_.paletteSrvHandle.second);
    commandList->SetComputeRootDescriptorTable(1, skinCluster_.inputVertexSrvHandle.second);
    commandList->SetComputeRootDescriptorTable(2, skinCluster_.influenceSrvHandle.second);
    commandList->SetComputeRootDescriptorTable(3, skinCluster_.outputVertexUavHandle.second);
    commandList->SetComputeRootConstantBufferView(4, skinCluster_.skinningInfoResource->GetGPUVirtualAddress());

    // スキニング実行
    int numVertices = static_cast<int>(model_->GetModelData().vertices.size());
    CSPipelineManager::GetInstance()->DisPatch(CSPipelineType::Skinning, commandList, numVertices);
}

///============================================================
/// 描画
///============================================================
void Object3DAnimation::Draw(const ViewProjection& viewProjection) {
    if (!model_ || !isDraw_) {
        return;
    }

    // WVPデータ更新
    UpdateWVPData(viewProjection);

    // アニメーション描画
    model_->DrawAnimation(wvpResource_, *shadowMap_, material_.get(), skinCluster_);
}

///=====================================================================
/// シャドウ描画
///=====================================================================
void Object3DAnimation::DrawShadow(const ViewProjection& viewProjection) {
    if (!model_ || !isShadow_ || !isDraw_) {
        return;
    }

    // WVPデータ更新
    UpdateWVPData(viewProjection);

    // スキン済み頂点バッファでシャドウマップ描画
    model_->DrawForShadowMapSkinned(wvpResource_, *shadowMap_, skinCluster_.outputVertexBufferView);
}

///============================================================
/// デバッグ描画
///============================================================
void Object3DAnimation::DebugLineSet() {
    for (const Joint& joint : skeleton_.joints) {
        // Joint位置
        Vector3 jointPos = TransformMatrix(transform_.GetWorldPos(), joint.skeletonSpaceMatrix);
        line3dDrawer_->SetCubeWireframe(jointPos, Vector3(0.01f, 0.01f, 0.01f), Vector4::kWHITE());

        // 親とのライン描画
        if (joint.parent) {
            const Joint& parentJoint = skeleton_.joints[*joint.parent];
            Vector3 parentPos        = TransformMatrix(transform_.GetWorldPos(), parentJoint.skeletonSpaceMatrix);
            line3dDrawer_->SetLine(jointPos, parentPos, Vector4::kWHITE());
        }
    }
}

///============================================================
/// 計算メソッド
///============================================================
Vector3 Object3DAnimation::CalculateValue(const std::vector<KeyframeVector3>& keyframe, float time) {
    return modelAnimation_->CalculateValue(keyframe, time);
}

Quaternion Object3DAnimation::CalculateValueQuaternion(const std::vector<KeyframeQuaternion>& keyframe, float time) {
    return modelAnimation_->CalculateValueQuaternion(keyframe, time);
}

///============================================================
/// Getter methods
///============================================================
const Joint* Object3DAnimation::GetJoint(const std::string& name) const {
    auto it = skeleton_.jointMap.find(name);
    if (it != skeleton_.jointMap.end()) {
        return &skeleton_.joints[it->second];
    }
    return nullptr;
}

const std::string& Object3DAnimation::GetCurrentAnimationName() const {
    static const std::string empty = "";
    if (animations_.empty())
        return empty;
    return animations_[currentAnimationIndex_].name;
}


std::vector<std::string> Object3DAnimation::GetAnimationNames() const {
    std::vector<std::string> names;
    for (const auto& anime : animations_) {
        names.push_back(anime.name);
    }
    return names;
}
