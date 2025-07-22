#pragma once

#include "BaseOffScreen.h"
#include"Matrix4x4.h"
#include <d3d12.h>
#include <wrl/client.h>

struct OutLineParamData {
    float wightRate;
};

struct OutLineMaterial {
    Matrix4x4 viewProjectionInverse;
};

class Outline : public BaseOffScreen {
private:
    void CreateGraphicsPipeline() override;
    void CreateRootSignature() override;

public:
    Outline()             = default;
    ~Outline() override = default;

    void Init(DirectXCommon* dxCommon) override;
    void SetDrawState(ID3D12GraphicsCommandList* commandList) override;

    void CreateConstantBuffer() override;
    void SetCommand([[maybe_unused]] ID3D12GraphicsCommandList* commandList) override;
    void DebugParamImGui() override;

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> uvStepResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> outlineMaterialResource_;
    OutLineParamData* uvStepData_;
    OutLineMaterial* outlineMaterialData_;
};
