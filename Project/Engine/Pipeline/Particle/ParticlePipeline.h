#pragma once
#include "../BasePipeline.h"
#include "base/SrvManager.h"
#include "Dx/DirectXCommon.h"
#include "Material/ModelMaterial.h"
#include <d3dcommon.h>
#include <dxcapi.h>

class ParticlePipeline : public BasePipeline {
public:
    ParticlePipeline()  = default;
    ~ParticlePipeline() = default;

    ///==============================================
    /// public method
    ///==============================================

    // 初期化
    void Init(DirectXCommon* dxCommon) override;
    void PreDraw(ID3D12GraphicsCommandList* commandList) override;
    void PreBlendSet(ID3D12GraphicsCommandList* commandList, const BlendMode& blendMode) override;

private:
    // ルートシグネチャの作成
    void CreateRootSignature() override;
    // グラフィックスパイプラインの生成
    void CreateGraphicsPipeline() override;

private:
    // other class
    SrvManager* pSrvManager_;

    // particle
    D3D12_STATIC_SAMPLER_DESC staticSamplers_[1];

    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateAdd_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateNone_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateMultiply_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateSubtractive_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateScreen_;

public:
    // getter
    ID3D12PipelineState* GetGrahipcsPipeLileStateAdd() const { return graphicsPipelineStateAdd_.Get(); }
    ID3D12PipelineState* GetGrahipcsPipeLileStateNone() const { return graphicsPipelineStateNone_.Get(); }
};