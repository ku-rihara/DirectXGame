#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <d3dcommon.h>
#include<dxcapi.h>

// class

class DirectXCommon;

class Line3DPipeline {
public:
    static Line3DPipeline* GetInstance();

    void Init(DirectXCommon* dxCommon);
    void PreDraw(ID3D12GraphicsCommandList* commandList);

private:
    Line3DPipeline() = default;
    void CreateRootSignature();
    void CreateGraphicsPipeline();

private:
    DirectXCommon* dxCommon_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;
    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;
    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob_;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob_;

    D3D12_STATIC_SAMPLER_DESC staticSamplers_[1] = {};
    D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};
};
