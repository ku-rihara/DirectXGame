#pragma once

#include <memory>
#include <unordered_set>
#include <vector>

// 前方宣言
class Object3DAnimation;
class ViewProjection;

/// <summary>
/// Animationの登録管理クラス
/// </summary>
class AnimationRegistry {
public:
    /// <summary>
    /// シングルトンインスタンス取得
    /// </summary>
    /// <returns>AnimationRegistryのインスタンス</returns>
    static AnimationRegistry* GetInstance();

    /// ============================================================
    /// public methods
    /// ============================================================

    /// <summary>
    /// アニメーションの登録
    /// </summary>
    /// <param name="animation">登録するアニメーションオブジェクト</param>
    void RegisterAnimation(Object3DAnimation* animation);

    /// <summary>
    /// アニメーションの登録解除
    /// </summary>
    /// <param name="animation">解除するアニメーションオブジェクト</param>
    void UnregisterAnimation(Object3DAnimation* animation);

    /// <summary>
    /// 登録された全てのアニメーションを更新
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    void UpdateAll(const float& deltaTime);

    /// <summary>
    /// 全アニメーションを描画
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション</param>
    void DrawAll(const ViewProjection& viewProjection);

    /// <summary>
    /// 全アニメーションのシャドウを描画
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション</param>
    void DrawAllShadow(const ViewProjection& viewProjection);

    void DebugLineAllSet();
    void Clear(); //< 登録されたアニメーションをクリア
    void DebugImGui(); //< ImGuiデバッグ表示

private:
    AnimationRegistry() = default;
    ~AnimationRegistry();

    /// コピー禁止
    AnimationRegistry(const AnimationRegistry&)            = delete;
    AnimationRegistry& operator=(const AnimationRegistry&) = delete;

private:
    std::unordered_set<Object3DAnimation*> animations_;
    static AnimationRegistry* instance_;
    static bool isDestroyed_;

public:
    const size_t& GetRegisteredCount() const { return animations_.size(); }
};