#pragma once

#include "DirectXCommon.h"

class SpriteCommon {
private://メンバ変数

    DirectXCommon* dxCommon_;

private://メンバ関数
    D3D12_STATIC_SAMPLER_DESC staticSamplers_[1];
    // グラフィックパイプライン関連

    // Sprite
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignatureSprite_;
    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlobSprite_;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlobSprite_;
    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlobSprite_;
    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlobSprite_;

    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateSprite_;

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

    ID3D12PipelineState* GetGraphicsPipelineStateSprite() const { return graphicsPipelineStateSprite_.Get(); }
    ID3D12RootSignature* GetRootSignatureSprite() const { return rootSignatureSprite_.Get(); }
};
