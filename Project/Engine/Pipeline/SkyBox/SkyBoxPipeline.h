#pragma once
#include "../BasePipeline.h"
#include "Dx/DirectXCommon.h"
#include <cstdint>
#include <dxcapi.h>
#include <string>

class SkyBoxPipeline : public BasePipeline {
public:
    SkyBoxPipeline()  = default;
    ~SkyBoxPipeline() = default;

    ///==============================================
    /// public method
    ///==============================================

    // 初期化
    void Init(DirectXCommon* dxCommon) override;
    void PreDraw(ID3D12GraphicsCommandList* commandList) override;
    void PreBlendSet(ID3D12GraphicsCommandList* commandList, const BlendMode& blendMode) override;

    // テクスチャ設定
    void SetEnvironmentalMapTextureHandle(const std::string& texture);

private:
    // ルートシグネチャの作成
    void CreateRootSignature() override;
    // グラフィックスパイプラインの生成
    void CreateGraphicsPipeline() override;

private:
    D3D12_STATIC_SAMPLER_DESC staticSamplers_[1];
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;
    uint32_t environmentalMapTextureHandle_;

public:
    // getter
    uint32_t GetEnvironmentalMapTextureHandle() const { return environmentalMapTextureHandle_; }
};