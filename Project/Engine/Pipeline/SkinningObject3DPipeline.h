#pragma once

#include "base/Material.h"
#include "Dx/DirectXCommon.h"
#include <dxcapi.h>
#include <d3dcommon.h>
#include <wrl/client.h>

class SkinningObject3DPipeline {

public:
    static SkinningObject3DPipeline* GetInstance();
    SkinningObject3DPipeline() = default;
    ~SkinningObject3DPipeline() = default;

    // 初期化
    void Init(DirectXCommon* dxCommon);

    // 共通描画処理
    void PreDraw(ID3D12GraphicsCommandList* commandList);
    void PreBlendSet(ID3D12GraphicsCommandList* commandList, BlendMode blendMode = BlendMode::None);

private:
    // ルートシグネチャの作成
    void CreateRootSignature();
    // グラフィックスパイプラインの生成
    void CreateGraphicsPipeline();

private: // メンバ変数
    DirectXCommon* dxCommon_;

private: // メンバ関数
    D3D12_STATIC_SAMPLER_DESC staticSamplers_[2];
    // グラフィックパイプライン関連
    // object
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob_;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob_;
    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;

    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateAdd_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateNone_;

    // depth
    D3D12_DEPTH_STENCIL_DESC depthStencilDesc_;

public:
    // getter
    DirectXCommon* GetDxCommon() const { return dxCommon_; }

    // rootSignature
    ID3D12PipelineState* GetGrahipcsPipeLileStateAdd() const { return graphicsPipelineStateAdd_.Get(); }
    ID3D12RootSignature* GetRootSignature() const { return rootSignature_.Get(); }
};
