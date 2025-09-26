#pragma once

#include "../BasePipeline.h"
#include "Dx/DirectXCommon.h"
#include "Material/ModelMaterial.h"
#include <d3dcommon.h>
#include <dxcapi.h>
#include <wrl/client.h>

class SkinningObject3DPipeline : public BasePipeline {
public:
    SkinningObject3DPipeline()  = default;
    ~SkinningObject3DPipeline() = default;

    // 初期化
    void Init(DirectXCommon* dxCommon) override;

    // 共通描画処理
    void PreDraw(ID3D12GraphicsCommandList* commandList) override;
    void PreBlendSet(ID3D12GraphicsCommandList* commandList, const BlendMode& blendMode) override;

protected:
    // ルートシグネチャの作成
    void CreateRootSignature() override;
    // グラフィックスパイプラインの生成
    void CreateGraphicsPipeline() override;

private: // メンバ変数
    D3D12_STATIC_SAMPLER_DESC staticSamplers_[2];

    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;

    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateAdd_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateNone_;

public:
    // getter
    ID3D12PipelineState* GetGrahipcsPipeLileStateAdd() const { return graphicsPipelineStateAdd_.Get(); }
};