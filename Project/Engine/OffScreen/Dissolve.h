#pragma once

#include "BaseOffScreen.h"
#include <d3d12.h>
#include <wrl/client.h>
#include <string>

class Dissolve : public BaseOffScreen {
public:
    struct ParamData {
        float thresholdValue;
    };

private:
    void CreateGraphicsPipeline() override;
    void CreateRootSignature() override;

public:
    Dissolve()             = default;
    ~Dissolve() override = default;

    void Init(DirectXCommon* dxCommon) override;
    void SetDrawState(ID3D12GraphicsCommandList* commandList) override;

    void CreateConstantBuffer() override;
    void Draw([[maybe_unused]] ID3D12GraphicsCommandList* commandList) override;
    void DebugParamImGui() override;

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> paramDataResource_;
    ParamData* paramData_;
    std::string maskTextureName_;
    uint32_t textureIndex_;
};
