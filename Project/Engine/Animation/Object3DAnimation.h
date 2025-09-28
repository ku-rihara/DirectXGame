#pragma once

#include "3d/BaseObject3d.h"
#include "3d/Line3d.h"
#include "Animation/SkeletonData.h"
#include "Animation/SkinCluster.h"
#include "AnimationData.h"
#include "ModelAnimation.h"
#include <memory>
#include <string>
#include <vector>
#include <Vector3.h>
#include <Quaternion.h>
#include <cstdint>

class Object3DAnimation : public BaseObject3d {
public:
    Object3DAnimation() = default;
    ~Object3DAnimation()override;

    /// ============================================================
    /// public methods
    /// ============================================================
   
    /// モデル作成
    static Object3DAnimation* CreateModel(const std::string& instanceName);

    // 初期化・リソース作成
    void Init();
    void Add(const std::string& fileName);

    // アニメーション制御
    void ChangeAnimation(const std::string& animationName);
    void SetAnimationTime(const float& time);
    void ResetAnimation();

    // 更新・描画
    void Update(const float& deltaTime);
    void Draw(const ViewProjection& viewProjection);
    void DrawShadow(const ViewProjection& viewProjection);
    void DebugDraw(const ViewProjection& viewProjection);
    void DebugImgui() override;

   

private:
    void Create(const std::string& fileName);
    Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframe, const float& time);
    Quaternion CalculateValueQuaternion(const std::vector<KeyframeQuaternion>& keyframe, const float& time);

private:
    /// ============================================================
    /// private methods
    /// ============================================================

    void UpdateAnimation(const float& deltaTime);
    void UpdateSkeleton();
    void UpdateSkinCluster();
    void AnimationTransition(const float& deltaTime);
    void TransitionFinish();
    void UpdateWVPData(const ViewProjection& viewProjection)override;
    void CreateWVPResource() override;
    void CreateMaterialResource() override;
    void CreateShadowMap() override;

private:
    /// ============================================================
    /// private members
    /// ============================================================

    // ModelAnimation
    std::unique_ptr<ModelAnimation> modelAnimation_;

    // アニメーションデータ
    std::vector<Animation> animations_;
    Skeleton skeleton_;
    SkinCluster skinCluster_;

    // 描画・デバッグ
    Line3D line3dDrawer_;

    // アニメーション状態
    float animationTime_           = 0.0f;
    int32_t preAnimationIndex_     = 0;
    int32_t currentAnimationIndex_ = 0;
    float currentTransitionTime_   = 0.0f;
    float preAnimationTime_        = 0.0f;
    bool isChange_                 = false;
    float transitionDuration_      = 0.3f;

public:
    /// ============================================================
    /// getter/setter methods
    /// ============================================================

    const Skeleton& GetSkeleton() const { return skeleton_; }
    const Joint* GetJoint(const std::string& name) const;
    const float& GetAnimationTime() const { return animationTime_; }
    float GetAnimationDuration() const;
    const int32_t& GetCurrentAnimationIndex() const { return currentAnimationIndex_; }
    const std::string& GetCurrentAnimationName() const;
    const float& IsAnimationTransitioning() const { return isChange_; }

    void SetTransitionDuration(const float& duration) { transitionDuration_ = duration; }
};