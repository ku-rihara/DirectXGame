#pragma once

#include "DirectXCommon.h"

class SpriteCommon {
private://メンバ変数

    DirectXCommon* dxCommon_;

private://メンバ関数
    D3D12_STATIC_SAMPLER_DESC staticSamplers_[1];
    // グラフィックパイプライン関連

    // Sprite
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob_;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob_;
    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;

    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;

    // ルートシグネチャの作成
    void CreateRootSignature();
    // グラフィックスパイプラインの生成
    void CreateGraphicsPipeline();

public://メンバ関数
    // 共通描画処理
    void PreDraw(ID3D12GraphicsCommandList* commandList);
    // 初期化
    void Init(DirectXCommon* dxCommon);

    static SpriteCommon* GetInstance();

    // getter
    DirectXCommon* GetDxCommon() const { return dxCommon_; }

    ID3D12PipelineState* GetGraphicsPipelineStateSprite() const { return graphicsPipelineState_.Get(); }
    ID3D12RootSignature* GetRootSignatureSprite() const { return rootSignature_.Get(); }
};