#pragma once

#include "BaseOffScreen.h"
#include <d3d12.h>

class GrayScale : public BaseOffScreen {
private:
    void CreateGraphicsPipeline() override;
    void CreateRootSignature() override;

public:
    GrayScale()           = default;
    ~GrayScale() override = default;

    void Init(DirectXCommon* dxCommon) override;
    void SetDrawState(ID3D12GraphicsCommandList* commandList) override;

    void CreateConstantBuffer() override;
    void SetCommand([[maybe_unused]] ID3D12GraphicsCommandList* commandList) override;
    void DebugParamImGui() override;

private:
};
