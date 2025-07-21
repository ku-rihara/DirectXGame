#pragma once

#include <d3d12.h>
#include <d3dcommon.h>
#include <dxcapi.h>
#include <string>
#include <wrl.h>
#include <wrl/client.h>

class DirectXCommon;

class BaseOffScreen {
protected:
    virtual void CreateGraphicsPipeline();
    virtual void CreateRootSignature();

public:
    BaseOffScreen()          = default;
    virtual ~BaseOffScreen() = default;

    virtual void Init(DirectXCommon* dxCommon);
    virtual void SetDrawState(ID3D12GraphicsCommandList* commandList);

protected:
    DirectXCommon* dxCommon_ = nullptr;

    D3D12_STATIC_SAMPLER_DESC staticSamplers_[1];
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;

    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStates_;

    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob_;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob_;

    std::wstring psName_ = L"resources/Shader/OffScreen/Fullscreen.PS.hlsl";
    std::wstring vsName_ = L"resources/Shader/OffScreen/Fullscreen.VS.hlsl";
};
