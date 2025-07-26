#pragma once

#include "BasePostEffect.h"
#include <d3d12.h>

class RandomNoize : public BasePostEffect {
public:
    struct RandomParamData {
        float time;
    };

private:
    void CreateGraphicsPipeline() override;
    void CreateRootSignature() override;

public:
    RandomNoize()           = default;
    ~RandomNoize() override = default;

    void Init(DirectXCommon* dxCommon) override;
    void SetDrawState(ID3D12GraphicsCommandList* commandList) override;

    void CreateConstantBuffer() override;
    void Draw([[maybe_unused]] ID3D12GraphicsCommandList* commandList) override;
    void DebugParamImGui() override;

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> paramDataResource_;
    RandomParamData* paramData_;
};
