#pragma once
#include "../BasePipeline.h"
#include "Dx/DirectXCommon.h"
#include <cstdint>
#include <dxcapi.h>
#include <string>

/// <summary>
/// スカイボックス描画パイプラインクラス
/// </summary>
namespace KetaEngine {

class SkyBoxPipeline : public BasePipeline {
public:
    SkyBoxPipeline()  = default;
    ~SkyBoxPipeline() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="dxCommon">DirectXCommon</param>
    void Init(DirectXCommon* dxCommon) override;

    /// <summary>
    /// 描画前処理
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    void PreDraw(ID3D12GraphicsCommandList* commandList) override;

    /// <summary>
    /// ブレンドモード設定
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    /// <param name="blendMode">ブレンドモード</param>
    void PreBlendSet(ID3D12GraphicsCommandList* commandList, const BlendMode& blendMode) override;

    /// <summary>
    /// 環境マップテクスチャの設定
    /// </summary>
    /// <param name="texture">テクスチャパス</param>
    void SetEnvironmentalMapTextureHandle(const std::string& texture);

private:
    void CreateRootSignature() override; //< ルートシグネチャ作成
    void CreateGraphicsPipeline() override; //< グラフィックスパイプライン作成

private:
    D3D12_STATIC_SAMPLER_DESC staticSamplers_[1];
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;
    uint32_t environmentalMapTextureHandle_;

public:
    uint32_t GetEnvironmentalMapTextureHandle() const { return environmentalMapTextureHandle_; }
};

}; // KetaEngine
