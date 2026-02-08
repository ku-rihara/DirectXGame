#pragma once

#include "3d/Object3D/BaseObject3d.h"
#include "3D/AnimationObject3D/SkeletonData.h"
#include "3D/AnimationObject3D/SkinCluster.h"
#include "AnimationData.h"
#include "3D/Line3D/Line3D.h"
#include "ModelAnimation.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <Quaternion.h>
#include <string>
#include <unordered_map>
#include <Vector3.h>
#include <vector>

/// <summary>
/// 3Dアニメーションオブジェクトクラス
/// </summary>
namespace KetaEngine {

class Object3DAnimation : public BaseObject3d {
public:
  
    Object3DAnimation() = default;
    ~Object3DAnimation() override;

    /// ============================================================
    /// public methods
    /// ============================================================

    void Init(); //< 初期化
    void ResetAnimation(); //< アニメーションリセット
    void DebugImGui() override; //< ImGuiデバッグ

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
    void SetAnimationTime(float time);

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    void Update(float deltaTime);

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
    Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframe, float time);

    /// <summary>
    /// キーフレーム補間計算(Quaternion)
    /// </summary>
    /// <param name="keyframe">キーフレーム配列</param>
    /// <param name="time">時間</param>
    /// <returns>補間された値</returns>
    Quaternion CalculateValueQuaternion(const std::vector<KeyframeQuaternion>& keyframe, float time);

    void CSSkinning(); //< コンピュートシェーダーによるスキニング

private:
    /// ============================================================
    /// private methods
    /// ============================================================

    /// <summary>
    /// アニメーション更新
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    void UpdateAnimation(float deltaTime);

    /// <summary>
    /// アニメーション遷移
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    void AnimationTransition(float deltaTime);

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

    void CreateWVPResource() override;      //< WVPリソース作成
    void CreateMaterialResource() override; //< マテリアルリソース作成
    void CreateShadowMap() override;        //< シャドウマップ作成

    void TransitionFinish();                //< 遷移終了
    void UpdateSkeleton();                  //< スケルトン更新
    void UpdateSkinCluster();               //< スキンクラスター更新

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
    bool wasPreAnimationFinished_  = false; // 前のアニメーションが終了状態だったか

    // ループ関連
    bool isLoop_             = true;
    bool hasLoopedThisFrame_ = false;

    // コールバック - アニメーション名をキーとしたマップ
    std::unordered_map<std::string, std::function<void()>> animationEndCallbacks_;

public:
    /// ============================================================
    /// getter/setter methods
    /// ============================================================

    const Skeleton& GetSkeleton() const { return skeleton_; }
    float GetAnimationTime() const { return animationTime_; }
    float GetAnimationDuration() const;
    int32_t GetCurrentAnimationIndex() const { return currentAnimationIndex_; }
    const std::string& GetCurrentAnimationName() const;
    bool IsAnimationTransitioning() const { return isChange_; }
    bool IsLoop() const { return isLoop_; }
    std::vector<std::string> GetAnimationNames() const;

    void SetTransitionDuration(float duration) { transitionDuration_ = duration; }
    void SetLoop(bool loop) { isLoop_ = loop; }
    
    /// <summary>
    /// 特定のアニメーション終了時のコールバックを設定
    /// </summary>
    /// <param name="animationName">アニメーション名</param>
    /// <param name="callback">コールバック関数</param>
    void SetAnimationEndCallback(const std::string& animationName, std::function<void()> callback) {
        animationEndCallbacks_[animationName] = callback;
    }

    /// <summary>
    /// 特定のアニメーションのコールバックを削除
    /// </summary>
    /// <param name="animationName">アニメーション名</param>
    void RemoveAnimationEndCallback(const std::string& animationName) {
        animationEndCallbacks_.erase(animationName);
    }

    /// <summary>
    /// 全てのアニメーション終了コールバックをクリア
    /// </summary>
    void ClearAllAnimationEndCallbacks() {
        animationEndCallbacks_.clear();
    }
};

} // KetaEngine
