#pragma once

#include "3d/BaseObject3d.h"
#include "Line3D/Line3D.h"
#include "Animation/SkeletonData.h"
#include "Animation/SkinCluster.h"
#include "AnimationData.h"
#include "ModelAnimation.h"
#include <cstdint>
#include <memory>
#include <Quaternion.h>
#include <string>
#include <Vector3.h>
#include <vector>

/// <summary>
/// 3Dアニメーションオブジェクトクラス
/// </summary>
class Object3DAnimation : public BaseObject3d {
public:
    Object3DAnimation() = default;
    ~Object3DAnimation() override;

    /// ============================================================
    /// public methods
    /// ============================================================
   
    void Init(); //< 初期化
    void ResetAnimation(); //< アニメーションリセット
    void DebugImgui() override; //< ImGuiデバッグ

    /// <summary>
    /// モデル作成
    /// </summary>
    /// <param name="instanceName">インスタンス名</param>
    /// <returns>作成されたアニメーションオブジェクト</returns>
    static Object3DAnimation* CreateModel(const std::string& instanceName);

    /// <summary>
    /// アニメーション追加
    /// </summary>
    /// <param name="fileName">ファイル名</param>
    void Add(const std::string& fileName);

    /// <summary>
    /// アニメーション変更
    /// </summary>
    /// <param name="animationName">アニメーション名</param>
    void ChangeAnimation(const std::string& animationName);

    /// <summary>
    /// アニメーション時間設定
    /// </summary>
    /// <param name="time">設定する時間</param>
    void SetAnimationTime(const float& time);

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    void Update(const float& deltaTime);

    /// <summary>
    /// 描画
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション</param>
    void Draw(const ViewProjection& viewProjection);

    /// <summary>
    /// シャドウ描画
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション</param>
    void DrawShadow(const ViewProjection& viewProjection);

    /// <summary>
    /// デバッグ描画
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション</param>
    void DebugLineSet();

private:
    /// <summary>
    /// 作成
    /// </summary>
    /// <param name="fileName">ファイル名</param>
    void Create(const std::string& fileName);

    /// <summary>
    /// キーフレーム補間計算(Vector3)
    /// </summary>
    /// <param name="keyframe">キーフレーム配列</param>
    /// <param name="time">時間</param>
    /// <returns>補間された値</returns>
    Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframe, const float& time);

    /// <summary>
    /// キーフレーム補間計算(Quaternion)
    /// </summary>
    /// <param name="keyframe">キーフレーム配列</param>
    /// <param name="time">時間</param>
    /// <returns>補間された値</returns>
    Quaternion CalculateValueQuaternion(const std::vector<KeyframeQuaternion>& keyframe, const float& time);

    void CSSkinning(); //< コンピュートシェーダーによるスキニング

private:
    /// ============================================================
    /// private methods
    /// ============================================================

    /// <summary>
    /// アニメーション更新
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    void UpdateAnimation(const float& deltaTime);

    /// <summary>
    /// アニメーション遷移
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    void AnimationTransition(const float& deltaTime);

    /// <summary>
    /// WVPデータ更新
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション</param>
    void UpdateWVPData(const ViewProjection& viewProjection) override;

    /// <summary>
    /// ジョイント取得
    /// </summary>
    /// <param name="name">ジョイント名</param>
    /// <returns>ジョイントのポインタ</returns>
    const Joint* GetJoint(const std::string& name) const;

    void CreateWVPResource() override; //< WVPリソース作成
    void CreateMaterialResource() override; //< マテリアルリソース作成
    void CreateShadowMap() override; //< シャドウマップ作成
    void TransitionFinish(); //< 遷移終了
    void UpdateSkeleton(); //< スケルトン更新
    void UpdateSkinCluster(); //< スキンクラスター更新

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
    std::unique_ptr<Line3D> line3dDrawer_;

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
    const float& GetAnimationTime() const { return animationTime_; }
    float GetAnimationDuration() const;
    const int32_t& GetCurrentAnimationIndex() const { return currentAnimationIndex_; }
    const std::string& GetCurrentAnimationName() const;
    const bool& IsAnimationTransitioning() const { return isChange_; }
    void SetTransitionDuration(const float& duration) { transitionDuration_ = duration; }
};