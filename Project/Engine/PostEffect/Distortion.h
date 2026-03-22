#pragma once

#include "BasePostEffect.h"
#include <d3d12.h>

namespace KetaEngine {

/// <summary>
/// 時空歪みポストエフェクト
/// </summary>
class Distortion : public BasePostEffect {
private:
    void CreateGraphicsPipeline() override;
    void CreateRootSignature() override;

public:
    Distortion()           = default;
    ~Distortion() override = default;

    void Init(DirectXCommon* dxCommon) override;
    void SetDrawState(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void CreateConstantBuffer() override;
    void DebugParamImGui() override;

    void SetDistortionSRV(D3D12_GPU_DESCRIPTOR_HANDLE handle) { distortionSrvHandle_ = handle; }

private:
    D3D12_GPU_DESCRIPTOR_HANDLE distortionSrvHandle_{};
};

} // namespace KetaEngine
