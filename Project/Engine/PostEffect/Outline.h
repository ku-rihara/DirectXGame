#pragma once

#include "BasePostEffect.h"
#include "Matrix4x4.h"
#include <d3d12.h>
#include <wrl/client.h>


class Outline : public BasePostEffect {
public:
    struct ParamData {
        float wightRate;
    };

    struct OutLineMaterial {
        Matrix4x4 projectionInverse;
    };

private:
    void CreateGraphicsPipeline() override;
    void CreateRootSignature() override;

public:
    Outline()           = default;
    ~Outline() override = default;

    void Init(DirectXCommon* dxCommon) override;
    void SetDrawState(ID3D12GraphicsCommandList* commandList) override;

    void CreateConstantBuffer() override;
    void Draw([[maybe_unused]] ID3D12GraphicsCommandList* commandList) override;
    void DebugParamImGui() override;

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> paramDataResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> outlineMaterialResource_;
    ParamData* paramData_;
    OutLineMaterial* outlineMaterialData_;

    D3D12_STATIC_SAMPLER_DESC staticSamplersOutLine_[2];

};