#pragma once

#include <unordered_set>

// 前方宣言
namespace KetaEngine {

class Object3DAnimation;
class ViewProjection;

/// <summary>
/// Animationの登録管理クラス
/// </summary>
class AnimationRegistry {
private:
    AnimationRegistry() = default;
    ~AnimationRegistry();

    // コピー禁止
    AnimationRegistry(const AnimationRegistry&)            = delete;
    AnimationRegistry& operator=(const AnimationRegistry&) = delete;

public:

    /// <summary>
    /// シングルトンインスタンスを取得する
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
    void UpdateAll(float deltaTime);

    /// <summary>
    /// 全アニメーションを描画
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション</param>
    void DrawAll(const ViewProjection& viewProjection);

    /// <summary>
    /// 全アニメーションのスキニングのみをまとめて実行する
    /// </summary>
    void SkinningAll();

    /// <summary>
    /// 全アニメーションのシャドウを描画
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション</param>
    void DrawAllShadow(const ViewProjection& viewProjection);

    /// <summary>
    /// 全アニメーションのデバッグライン表示を設定する
    /// </summary>
    void DebugLineAllSet();

    /// <summary>
    /// 登録されたアニメーションをクリアする
    /// </summary>
    void Clear();

    /// <summary>
    /// ImGuiデバッグ表示
    /// </summary>
    void DebugImGui();

private:
    std::unordered_set<Object3DAnimation*> animations_;
    static AnimationRegistry* instance_;
    static bool isDestroyed_;

public:
    /// <summary>
    /// 登録されているアニメーションの数を取得する
    /// </summary>
    /// <returns>登録数</returns>
    const size_t& GetRegisteredCount() const { return animations_.size(); }
};

}; // KetaEngine
