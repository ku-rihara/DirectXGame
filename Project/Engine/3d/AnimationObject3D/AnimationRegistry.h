#pragma once

#include "Utility/Registry/RegistryBase.h"

// 前方宣言
namespace KetaEngine {

class Object3DAnimation;
class ViewProjection;

/// <summary>
/// Animationの登録管理クラス
/// </summary>
class AnimationRegistry : public RegistryBase<AnimationRegistry, Object3DAnimation> {
public:
    AnimationRegistry()  = default;
    ~AnimationRegistry() = default;

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
    void UnregisterAnimation(Object3DAnimation* animation) { UnregisterObject(animation); }

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
    /// ImGuiデバッグ表示
    /// </summary>
    void DebugImGui();
};

}; // KetaEngine
