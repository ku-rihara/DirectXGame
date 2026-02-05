#pragma once
#include "BasePostEffect.h"
#include "GrayScale.h"
#include "RadialBlur.h"
#include "Vignette.h"
#include <array>
#include <d3d12.h>
#include <memory>
namespace KetaEngine {

class ViewProjection;

enum class PostEffectMode {
    NONE,
    GRAY,
    VIGNETTE,
    GAUS,
    BOXFILTER,
    RADIALBLUR,
    RANDOMNOISE,
    DISSOLVE,
    OUTLINE,
    LUMINANCEOUTLINE,
    COUNT,
};

class DirectXCommon;

/// <summary>
/// ポストエフェクト描画管理クラス
/// </summary>
class PostEffectRenderer {
public:
    ~PostEffectRenderer() = default;
    static PostEffectRenderer* GetInstance();

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="dxCommon">DirectXCommon</param>
    void Init(DirectXCommon* dxCommon);

    /// <summary>
    /// 描画
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    void Draw(ID3D12GraphicsCommandList* commandList);

    /// <summary>
    /// 指定したモードのエフェクト取得
    /// </summary>
    /// <param name="mode">ポストエフェクトモード</param>
    /// <returns>ベースポストエフェクト</returns>
    BasePostEffect* GetEffect(PostEffectMode mode);

    /// <summary>
    /// エフェクト取得
    /// </summary>
    /// <typeparam name="T">エフェクトの型</typeparam>
    /// <param name="mode">ポストエフェクトモード</param>
    /// <returns>指定した型のエフェクト</returns>
    template <typename T>
    T* GetEffect(PostEffectMode mode) {
        BasePostEffect* effect = GetEffect(mode);
        return dynamic_cast<T*>(effect);
    }

    /// <summary>
    /// 現在のモードのエフェクトを取得
    /// </summary>
    /// <typeparam name="T">エフェクトの型</typeparam>
    /// <returns>指定した型のエフェクト</returns>
    template <typename T>
    T* GetCurrentEffect() {
        return GetEffect<T>(currentMode_);
    }

    void DrawImGui(); //< ImGui描画

private:
    const ViewProjection* viewProjection_;
    DirectXCommon* dxCommon_    = nullptr;
    PostEffectMode currentMode_ = PostEffectMode::NONE;
    std::array<std::unique_ptr<BasePostEffect>, static_cast<size_t>(PostEffectMode::COUNT)> effects_;

public:
   // getter

    RadialBlur* GetRadialBlur() {
        return GetEffect<RadialBlur>(PostEffectMode::RADIALBLUR);
    }

    GrayScale* GetGrayScale() {
        return GetEffect<GrayScale>(PostEffectMode::GRAY);
    }

    Vignette* GetVignette() {
        return GetEffect<Vignette>(PostEffectMode::VIGNETTE);
    }

    // setter
    void SetViewProjection(const ViewProjection* viewProjection);
    void SetPostEffectMode(const PostEffectMode& mode) { currentMode_ = mode; }
};

}; // KetaEngine
