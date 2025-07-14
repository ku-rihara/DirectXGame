#pragma once

#include "BaseOffScreen.h"
#include <d3d12.h>

class RadialBlur : public BaseOffScreen {
private:
    void CreateGraphicsPipeline() override;
    void CreateRootSignature() override;

public:
    RadialBlur()           = default;
    ~RadialBlur() override = default;

    void Init(DirectXCommon* dxCommon) override;
    void SetDrawState(ID3D12GraphicsCommandList* commandList) override;

private:
};
