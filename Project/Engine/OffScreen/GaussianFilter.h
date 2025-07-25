#pragma once

#include "BaseOffScreen.h"
#include <d3d12.h>

class GaussianFilter : public BaseOffScreen {
public:
    struct ParamData {
        float sigma;
    };

private:
    void CreateGraphicsPipeline() override;
    void CreateRootSignature() override;

public:
    GaussianFilter()           = default;
    ~GaussianFilter() override = default;

    void Init(DirectXCommon* dxCommon) override;
    void SetDrawState(ID3D12GraphicsCommandList* commandList) override;

    void CreateConstantBuffer() override;
    void Draw([[maybe_unused]] ID3D12GraphicsCommandList* commandList) override;
    void DebugParamImGui() override;

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> paramDataResource_;
    ParamData* paramData_;

};
