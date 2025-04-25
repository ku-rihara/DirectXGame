#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <dxcapi.h>
#include <d3dcommon.h>

class DirectXCommon;
class CopyImageRenderer {
private:
   
    DirectXCommon* dxCommon_ = nullptr;
    D3D12_STATIC_SAMPLER_DESC staticSamplers_[1];
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;
    // シリアライズしてルートシグネチャを作成
    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob_;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob_;
private:
    void CreateGraphicsPipeline();
    void CreateRootSignature();

public:
    static CopyImageRenderer* GetInstance();

    void Init(DirectXCommon* dxCommon);
    void Draw(ID3D12GraphicsCommandList* commandList);



};
