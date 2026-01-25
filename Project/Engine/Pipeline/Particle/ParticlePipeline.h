#pragma once
#include "../BasePipeline.h"
#include "Base/Descriptors/SrvManager.h"
#include "Base/Dx/DirectXCommon.h"
#include "Base/Material/ModelMaterial.h"
#include <d3dcommon.h>
#include <dxcapi.h>

/// <summary>
/// パーティクル描画パイプラインクラス
/// </summary>
namespace KetaEngine {

enum class ParticleRootParameter {
    ParticleData,
    Material,    
    Texture,     
    Count
};

class ParticlePipeline : public BasePipeline {
public:
    ParticlePipeline()  = default;
    ~ParticlePipeline() = default;

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
    SrvManager* pSrvManager_;

    D3D12_STATIC_SAMPLER_DESC staticSamplers_[1];

    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateAdd_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateNone_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateMultiply_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateSubtractive_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateScreen_;

public:
    ID3D12PipelineState* GetGrahipcsPipeLileStateAdd() const { return graphicsPipelineStateAdd_.Get(); }
    ID3D12PipelineState* GetGrahipcsPipeLileStateNone() const { return graphicsPipelineStateNone_.Get(); }
};

}; // KetaEngine
