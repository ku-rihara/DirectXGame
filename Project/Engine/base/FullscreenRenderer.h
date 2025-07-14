#pragma once

#include <array>
#include <cstdint>
#include <d3d12.h>
#include <d3dcommon.h>
#include <dxcapi.h>
#include <string>
#include <wrl.h>

enum class OffScreenMode {
    NONE,
    GRAY,
    VIGNETTE,
    GAUS,
    BOXFILTER,
    COUNT,
};

class DirectXCommon;

class FullscreenRenderer {
private:
    void CreateGraphicsPipeline();
    void CreateRootSignature();

public:

    ~FullscreenRenderer() = default;
    static FullscreenRenderer* GetInstance();

    void Init(DirectXCommon* dxCommon);
    void Draw(ID3D12GraphicsCommandList* commandList);
    void DrawImGui();

    void SetOffScreenMode(const OffScreenMode& mode) { currentMode_ = mode; }

private:
    DirectXCommon* dxCommon_ = nullptr;
    D3D12_STATIC_SAMPLER_DESC staticSamplers_[1];
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;

    std::array<Microsoft::WRL::ComPtr<IDxcBlob>, static_cast<size_t>(OffScreenMode::COUNT)> pixelShaderBlob_;
    std::array<std::wstring, static_cast<size_t>(OffScreenMode::COUNT)> psFiles_;
    std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, static_cast<size_t>(OffScreenMode::COUNT)> pipelineStates_;

    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob_;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob_;

    OffScreenMode currentMode_ = OffScreenMode::NONE;
};
