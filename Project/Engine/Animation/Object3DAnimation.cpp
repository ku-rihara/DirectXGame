#include "Object3DAnimation.h"
#include "3d/ModelManager.h"
#include "AnimationRegistry.h"
#include "base/SrvManager.h"
#include "MathFunction.h"
#include "Pipeline/Object3D/Object3DPipeline.h"
#include "Pipeline/PipelineManager.h"
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
    SetModel(fileName);

    // アニメーションデータの読み込み
    animations_.push_back(modelAnimation_->LoadAnimationFile(fileName));

    // スケルトンの作成
    if (model_) {
        skeleton_ = modelAnimation_->CreateSkeleton(model_->GetModelData().rootNode);

        // スキンクラスターの作成
        ModelData modelData = model_->GetModelData();
        skinCluster_        = modelAnimation_->CreateSkinCluster(modelData, skeleton_);
    }

    // リソース作成
    CreateWVPResource();
    CreateMaterialResource();
    CreateShadowMap();

    Init();
}

///============================================================
/// 初期化
///============================================================
void Object3DAnimation::Init() {
    transform_.Init();
    line3dDrawer_.Init(5120);
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

            // 切り替え変数リセット
            animationTime_         = 0.0f;
            currentTransitionTime_ = 0.0f;
            isChange_              = true;

            return;
        }
    }
}

///============================================================
/// アニメーション時間設定
///============================================================
void Object3DAnimation::SetAnimationTime(const float& time) {
    if (!animations_.empty()) {
        float duration = animations_[currentAnimationIndex_].duration;
        animationTime_ = std::fmod(time, duration);
    }
}

///============================================================
/// アニメーションリセット
///============================================================
void Object3DAnimation::ResetAnimation() {
    animationTime_         = 0.0f;
    currentTransitionTime_ = 0.0f;
    isChange_              = false;
}

///============================================================
/// 更新
///============================================================
void Object3DAnimation::Update(const float& deltaTime) {
    if (animations_.empty()) {
        return;
    }

    UpdateAnimation(deltaTime);
    UpdateSkeleton();
    UpdateSkinCluster();

    // Transform更新
    transform_.UpdateMatrix();

    // Object3dの更新処理
    material_.materialData_->color = objColor_.GetColor();
}

///============================================================
/// アニメーション更新
///============================================================
void Object3DAnimation::UpdateAnimation(const float& deltaTime) {
    animationTime_ += deltaTime;
    animationTime_ = std::fmod(animationTime_, animations_[currentAnimationIndex_].duration);

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

        skinCluster_.mappedPalette[jointIndex].skeletonSpaceMatrix =
            skinCluster_.inverseBindPoseMatrices[jointIndex] * skeleton_.joints[jointIndex].skeletonSpaceMatrix;
        skinCluster_.mappedPalette[jointIndex].skeletonSpaceInverseTransposeMatrix =
            Inverse(Transpose(skinCluster_.mappedPalette[jointIndex].skeletonSpaceMatrix));
    }
}

///============================================================
/// アニメーション遷移
///============================================================
void Object3DAnimation::AnimationTransition(const float& deltaTime) {
    // 補間タイム加算
    currentTransitionTime_ += deltaTime / transitionDuration_;
    preAnimationTime_ += deltaTime;
    currentTransitionTime_ = std::min(currentTransitionTime_, 1.0f);

    // 前のアニメーションタイム
    float preTime = std::fmod(preAnimationTime_, animations_[preAnimationIndex_].duration);

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
    currentTransitionTime_ = 0.0f;
    isChange_              = false;
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

    // スキニング用パイプライン設定
    PipelineManager::GetInstance()->PreDraw(PipelineType::SkinningObject3D, DirectXCommon::GetInstance()->GetCommandList());

    // アニメーション描画
    model_->DrawAnimation(wvpResource_, *shadowMap_, material_, skinCluster_);

    // 通常パイプラインに戻す
    PipelineManager::GetInstance()->PreDraw(PipelineType::Object3D, DirectXCommon::GetInstance()->GetCommandList());
}

///=====================================================================
/// シャドウ描画
///=====================================================================
void Object3DAnimation::DrawShadow(const ViewProjection& viewProjection) {
    if (!model_ || !isDraw_) {
        return;
    }

    // WVPデータ更新
    UpdateWVPData(viewProjection);

    // シャドウマップ用描画
    model_->DrawForShadowMap(wvpResource_, *shadowMap_);
}

///============================================================
/// デバッグ描画
///============================================================
void Object3DAnimation::DebugDraw(const ViewProjection& viewProjection) {
    for (const Joint& joint : skeleton_.joints) {
        // Joint位置
        Vector3 jointPos = TransformMatrix(transform_.GetWorldPos(), joint.skeletonSpaceMatrix);
        line3dDrawer_.DrawCubeWireframe(jointPos, Vector3(0.01f, 0.01f, 0.01f), Vector4::kWHITE());

        // 親とのライン描画
        if (joint.parent) {
            const Joint& parentJoint = skeleton_.joints[*joint.parent];
            Vector3 parentPos        = TransformMatrix(transform_.GetWorldPos(), parentJoint.skeletonSpaceMatrix);
            line3dDrawer_.SetLine(jointPos, parentPos, Vector4::kWHITE());
        }
    }

    // Joint描画
    line3dDrawer_.Draw(viewProjection);
}

///============================================================
/// デバッグImGui
///============================================================
void Object3DAnimation::DebugImgui() {
    BaseObject3d::DebugImgui();
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

float Object3DAnimation::GetAnimationDuration() const {
    if (animations_.empty())
        return 0.0f;
    return animations_[currentAnimationIndex_].duration;
}

const std::string& Object3DAnimation::GetCurrentAnimationName() const {
    static const std::string empty = "";
    if (animations_.empty())
        return empty;
    return animations_[currentAnimationIndex_].name;
}

///============================================================
/// 計算メソッド
///============================================================
Vector3 Object3DAnimation::CalculateValue(const std::vector<KeyframeVector3>& keyframe, const float& time) {
    return modelAnimation_->CalculateValue(keyframe, time);
}

Quaternion Object3DAnimation::CalculateValueQuaternion(const std::vector<KeyframeQuaternion>& keyframe, const float& time) {
    return modelAnimation_->CalculateValueQuaternion(keyframe, time);
}

///============================================================
/// WVP更新
///============================================================
void Object3DAnimation::UpdateWVPData(const ViewProjection& viewProjection) {
    BaseObject3d::UpdateWVPData(viewProjection);
}

void Object3DAnimation::CreateWVPResource() {
    BaseObject3d::CreateWVPResource();
}

void Object3DAnimation::CreateMaterialResource() {
    BaseObject3d::CreateMaterialResource();
}

void Object3DAnimation::CreateShadowMap() {
    BaseObject3d::CreateShadowMap();
}