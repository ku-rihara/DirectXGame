#pragma once
#include "BasePostEffect.h"
#include <array>
#include <d3d12.h>
#include <memory>
#include "RadialBlur.h"
#include "GrayScale.h"
#include "Vignette.h"
class ViewProjection;
enum class PostEffectMode {
    NONE,
    GRAY,
    VIGNETTE,
    GAUS,
    BOXFILTER,
    RADIALBLUR,
    RANDOMNOIZE,
    DISSOLVE,
    OUTLINE,
    LUMINANCEOUTLINE,
    COUNT,
};

class DirectXCommon;

class PostEffectRenderer {
public:
    ~PostEffectRenderer() = default;
    static PostEffectRenderer* GetInstance();

    void Init(DirectXCommon* dxCommon);
    void Draw(ID3D12GraphicsCommandList* commandList);
    void DrawImGui();

    void SetPostEffectMode(const PostEffectMode& mode) { currentMode_ = mode; }

private:
    const ViewProjection* viewProjection_;
    DirectXCommon* dxCommon_   = nullptr;
    PostEffectMode currentMode_ = PostEffectMode::NONE;
    std::array<std::unique_ptr<BasePostEffect>, static_cast<size_t>(PostEffectMode::COUNT)> effects_;

public:
    void SetViewProjection(const ViewProjection* viewProjection);
    BasePostEffect* GetEffect(PostEffectMode mode);

     // テンプレート版の型安全な取得関数
    template <typename T>
    T* GetEffect(PostEffectMode mode) {
        BasePostEffect* effect = GetEffect(mode);
        return dynamic_cast<T*>(effect);
    }

    // 現在のモードの効果を型安全に取得
    template <typename T>
    T* GetCurrentEffect() {
        return GetEffect<T>(currentMode_);
    }

    // 特定の効果専用の便利関数
    RadialBlur* GetRadialBlur() {
        return GetEffect<RadialBlur>(PostEffectMode::RADIALBLUR);
    }

    GrayScale* GetGrayScale() {
        return GetEffect<GrayScale>(PostEffectMode::GRAY);
    }

    Vignette* GetVignette() {
        return GetEffect<Vignette>(PostEffectMode::VIGNETTE);
    }
};
