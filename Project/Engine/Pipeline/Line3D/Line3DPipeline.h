#pragma once
#include "../BasePipeline.h"
#include <d3d12.h>
#include <d3dcommon.h>
#include <dxcapi.h>
#include <wrl/client.h>

class Line3DPipeline : public BasePipeline {
public:
    Line3DPipeline()  = default;
    ~Line3DPipeline() = default;

 
    void Init(DirectXCommon* dxCommon) override;
    void PreDraw(ID3D12GraphicsCommandList* commandList) override;
    void PreBlendSet(ID3D12GraphicsCommandList* commandList, const BlendMode& blendMode) override;

protected:
    void CreateRootSignature() override;
    void CreateGraphicsPipeline() override;

private:
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;
    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;
};