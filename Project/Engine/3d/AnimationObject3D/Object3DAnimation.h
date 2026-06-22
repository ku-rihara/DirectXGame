#pragma once

// 3D
#include "3D/AnimationObject3D/SkeletonData.h"
#include "3D/AnimationObject3D/SkinCluster.h"
#include "3D/Line3D/Line3D.h"
#include "3D/Object3D/BaseObject3d.h"
// Aniamtion
#include "AnimationData.h"
#include "ModelAnimation.h"
// math
#include "Quaternion.h"
#include "Vector3.h"
// std
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

/// <summary>
/// 3Dアニメーションオブジェクトクラス
/// </summary>
namespace KetaEngine {

class Object3DAnimation : public BaseObject3d {
public:
    Object3DAnimation() = default;
    ~Object3DAnimation() override;

    ///============================================================
    /// 生成・アニメーション追加
    ///============================================================

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

    ///============================================================
    /// 更新・描画
    ///============================================================

    void Init(); //< 初期化

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

    void CSSkinning(); //< CSスキニング

    ///============================================================
    /// デバッグ
    ///============================================================

    void DebugLineSet(); //< デバッグ用ライン設定

    ///============================================================
    /// アニメーション終了コールバック
    ///============================================================

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

private:
    ///============================================================
    /// private methods
    ///============================================================

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
    /// ジョイント取得
    /// </summary>
    /// <param name="name">ジョイント名</param>
    /// <returns>ジョイントのポインタ</returns>
    const Joint* GetJoint(const std::string& name) const;

    void TransitionFinish(); //< 遷移終了
    void UpdateSkeleton(); //< スケルトン更新
    void UpdateSkinCluster(); //< スキンクラスター更新

private:
    ///============================================================
    /// private members
    ///============================================================

    // アニメーションデータ
    std::unique_ptr<ModelAnimation> modelAnimation_;
    std::vector<Animation> animations_;
    Skeleton skeleton_;
    SkinCluster skinCluster_;

    // 描画・デバッグ
    std::unique_ptr<Line3D> line3dDrawer_;

    // 再生状態
    int32_t currentAnimationIndex_ = 0; //< 現在のアニメーション番号
    float animationTime_           = 0.0f; //< 現在の再生時間

    // 遷移関連
    bool isChange_                = false; //< 遷移中かどうか
    float transitionDuration_     = 0.3f; //< 遷移にかける時間
    float currentTransitionTime_  = 0.0f; //< 遷移の経過時間
    int32_t preAnimationIndex_    = 0; //< 遷移元のアニメーション番号
    float preAnimationTime_       = 0.0f; //< 遷移元の再生時間
    bool wasPreAnimationFinished_ = false; //< 遷移元が終了済みだったか

    // ループ
    bool isLoop_             = true; //< ループ再生するか
    bool hasLoopedThisFrame_ = false; //< このフレームでループしたか

    // 非アクティブ時はUpdateAll/DrawAllでスキップする
    bool isActive_ = true;

    // アニメーション終了コールバック
    std::unordered_map<std::string, std::function<void()>> animationEndCallbacks_;

public:
    ///============================================================
    /// getter
    ///============================================================

    const Skeleton& GetSkeleton() const { return skeleton_; }
    const std::string& GetCurrentAnimationName() const;
    std::vector<std::string> GetAnimationNames() const;
    bool IsActive() const { return isActive_; }
    ID3D12Resource* GetOutputVertexResource() const { return skinCluster_.outputVertexResource.Get(); }

    ///============================================================
    /// setter
    ///============================================================

    void SetLoop(bool loop) { isLoop_ = loop; }
    void SetIsActive(bool active) {
        isActive_ = active;
        isDraw_   = active;
    }
};

} // KetaEngine
