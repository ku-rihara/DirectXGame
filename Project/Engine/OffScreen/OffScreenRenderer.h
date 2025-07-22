#pragma once
#include"BaseOffScreen.h"
#include <array>
#include <d3d12.h>
#include <memory>

enum class OffScreenMode {
    NONE,
    GRAY,
    VIGNETTE,
    GAUS,
    BOXFILTER,
    RADIALBLUR,
    OUTLINE,
    COUNT,
};

class DirectXCommon;

class OffScreenRenderer {
public:
    ~OffScreenRenderer() = default;
    static OffScreenRenderer* GetInstance();

    void Init(DirectXCommon* dxCommon);
    void Draw(ID3D12GraphicsCommandList* commandList);
    void DrawImGui();

    void SetOffScreenMode(const OffScreenMode& mode) { currentMode_ = mode; }

private:
    DirectXCommon* dxCommon_ = nullptr;
    OffScreenMode currentMode_ = OffScreenMode::NONE;
    std::array<std::unique_ptr<BaseOffScreen>, static_cast<size_t>(OffScreenMode::COUNT)> effects_;
};
