#pragma once

#include "../BasePipeline.h"
#include "Dx/DirectXCommon.h"
#include <d3dcommon.h>
#include <dxcapi.h>
#include <wrl/client.h>

/// <summary>
/// シャドウマップ描画パイプラインクラス
/// </summary>
class ShadowMapPipeline : public BasePipeline {
public:
    ShadowMapPipeline()  = default;
    ~ShadowMapPipeline() = default;

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
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;

public:
    ID3D12PipelineState* GetPipelineState() const { return pipelineState_.Get(); }
};