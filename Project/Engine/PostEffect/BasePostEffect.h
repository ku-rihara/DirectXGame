#pragma once

#include <d3d12.h>
#include <d3dcommon.h>
#include <dxcapi.h>
#include <string>
#include <wrl.h>
#include <wrl/client.h>

class DirectXCommon;
class ViewProjection;

class BasePostEffect {
protected:
    virtual void CreateGraphicsPipeline();
    virtual void CreateRootSignature();

public:
    BasePostEffect()          = default;
    virtual ~BasePostEffect() = default;

    virtual void Init(DirectXCommon* dxCommon);
    virtual void SetDrawState(ID3D12GraphicsCommandList* commandList);

    virtual void Draw([[maybe_unused]] ID3D12GraphicsCommandList* commandList) = 0;
    virtual void CreateConstantBuffer()                                        = 0;
    virtual void DebugParamImGui()                                             = 0;

protected:
    DirectXCommon* dxCommon_ = nullptr;
    const ViewProjection* viewProjection_;

    D3D12_STATIC_SAMPLER_DESC staticSamplers_[1];
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;

    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStates_;

    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob_;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob_;

    std::wstring psName_ = L"resources/Shader/PostEffect/Fullscreen.PS.hlsl";
    std::wstring vsName_ = L"resources/Shader/PostEffect/Fullscreen.VS.hlsl";

public:
    void SetViewProjection(const ViewProjection* viewProjection);
};
