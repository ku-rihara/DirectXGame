#pragma once

#include "../BasePipeline.h"
#include "Dx/DirectXCommon.h"
#include "Material/ModelMaterial.h"
#include <d3dcommon.h>
#include <dxcapi.h>
#include <wrl/client.h>

/// <summary>
/// 3Dオブジェクト描画パイプラインクラス
/// </summary>
class Object3DPipeline : public BasePipeline {
public:
    Object3DPipeline()  = default;
    ~Object3DPipeline() = default;

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

protected:
    void CreateRootSignature() override; //< ルートシグネチャ作成
    void CreateGraphicsPipeline() override; //< グラフィックスパイプライン作成

private:
    D3D12_STATIC_SAMPLER_DESC staticSamplers_[2];

    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateAdd_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateNone_;

public:
    ID3D12PipelineState* GetGrahipcsPipeLileStateAdd() const { return graphicsPipelineStateAdd_.Get(); }
};