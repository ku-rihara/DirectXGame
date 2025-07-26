#pragma once

#include "BasePostEffect.h"
#include "Matrix4x4.h"
#include <d3d12.h>
#include <wrl/client.h>


class LuminanceBasedOutline : public BasePostEffect {
public:
    struct ParamData {
        float wightRate;
    };

private:
    void CreateGraphicsPipeline() override;
    void CreateRootSignature() override;

public:
    LuminanceBasedOutline() = default;
    ~LuminanceBasedOutline() override = default;

    void Init(DirectXCommon* dxCommon) override;
    void SetDrawState(ID3D12GraphicsCommandList* commandList) override;

    void CreateConstantBuffer() override;
    void Draw([[maybe_unused]] ID3D12GraphicsCommandList* commandList) override;
    void DebugParamImGui() override;

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> paramDataResource_;
    ParamData* paramData_;


};