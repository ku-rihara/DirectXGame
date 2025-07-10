#pragma once

#include"Dx/DirectXCommon.h"
#include <cstdint>
#include <string>

class SkyBoxRenderer {
public: 
    SkyBoxRenderer() = default;
    ~SkyBoxRenderer() = default;

    // 共通描画処理
    void PreDraw(ID3D12GraphicsCommandList* commandList);
    void SetPiplelineState(ID3D12GraphicsCommandList* commandList);
    // 初期化
    void Init(DirectXCommon* dxCommon);

    static SkyBoxRenderer* GetInstance();

    // rootSignature
    ID3D12RootSignature* GetRootSignature() const { return rootSignature_.Get(); }
    uint32_t GetEnvironmentalMapTextureHandle() const { return environmentalMapTextureHandle_; }

    void SetEnvironmentalMapTextureHandle(const std::string& texture);

private:
    DirectXCommon* dxCommon_;

private: 

    D3D12_STATIC_SAMPLER_DESC staticSamplers_[1];
    // グラフィックパイプライン関連
    // object
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob_;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob_;
    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;

    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;

    uint32_t environmentalMapTextureHandle_;

    // depth
    D3D12_DEPTH_STENCIL_DESC depthStencilDesc_;

private:
    // ルートシグネチャの作成
    void CreateRootSignature();
    // グラフィックスパイプラインの生成
    void CreateGraphicsPipeline();


};
