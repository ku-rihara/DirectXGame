#pragma once

#include "BaseOffScreen.h"
#include <d3d12.h>

class Vignette : public BaseOffScreen {
private:
    void CreateGraphicsPipeline() override;
    void CreateRootSignature() override;

public:
    Vignette()           = default;
    ~Vignette() override = default;

    void Init(DirectXCommon* dxCommon) override;
    void SetDrawState(ID3D12GraphicsCommandList* commandList) override;

    void CreateConstantBuffer() override;
    void SetCommand([[maybe_unused]] ID3D12GraphicsCommandList* commandList) override;
    void DebugParamImGui() override;

private:
};
