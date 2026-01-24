#pragma once
#include "../BasePipeline.h"
#include "Base/Dx/DirectXCommon.h"
#include <dxcapi.h>

/// <summary>
/// スプライト描画パイプラインクラス
/// </summary>
namespace KetaEngine {

class SpritePipeline : public BasePipeline {
public:
    SpritePipeline()  = default;
    ~SpritePipeline() = default;

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

private:
    void CreateRootSignature() override; //< ルートシグネチャ作成
    void CreateGraphicsPipeline() override; //< グラフィックスパイプライン作成

private:
    D3D12_STATIC_SAMPLER_DESC staticSamplers_[1];

    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;

    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;

public:
    ID3D12PipelineState* GetGraphicsPipelineStateSprite() const { return graphicsPipelineState_.Get(); }
};

}; // KetaEngine
