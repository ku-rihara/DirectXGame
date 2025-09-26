#pragma once
#include "Dx/DirectXCommon.h"
#include "../BasePipeline.h"
#include <dxcapi.h>

class SpritePipeline : public BasePipeline {
public:
   
    SpritePipeline()  = default;
    ~SpritePipeline() = default;

    // 初期化
    void Init(DirectXCommon* dxCommon) override;
    // 共通描画処理
    void PreDraw(ID3D12GraphicsCommandList* commandList) override;
    // Blend Set
    void PreBlendSet(ID3D12GraphicsCommandList* commandList, const BlendMode& blendMode) override;

private:
    // ルートシグネチャの作成
    void CreateRootSignature() override;
    // グラフィックスパイプラインの生成
    void CreateGraphicsPipeline() override;

private: // メンバ変数
    D3D12_STATIC_SAMPLER_DESC staticSamplers_[1];
  
  
    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;

    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;

public:
    // getter
    ID3D12PipelineState* GetGraphicsPipelineStateSprite() const { return graphicsPipelineState_.Get(); }
};