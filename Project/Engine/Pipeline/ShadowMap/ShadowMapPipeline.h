#pragma once

#include "Dx/DirectXCommon.h"
#include "../BasePipeline.h"
#include <d3dcommon.h>
#include <dxcapi.h>
#include <wrl/client.h>

class ShadowMapPipeline : public BasePipeline {
public:
  
    ShadowMapPipeline()  = default;
    ~ShadowMapPipeline() = default;

    // 初期化
    void Init(DirectXCommon* dxCommon) override;
    // 共通描画処理
    void PreDraw(ID3D12GraphicsCommandList* commandList) override;
    // Blend Set
    void PreBlendSet(ID3D12GraphicsCommandList* commandList, const BlendMode& blendMode) override;

protected:
    // ルートシグネチャの作成
    void CreateRootSignature() override;
    // グラフィックスパイプラインの生成
    void CreateGraphicsPipeline() override;

private: 

    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;

    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;

public:
    // getter
    ID3D12PipelineState* GetPipelineState() const { return pipelineState_.Get(); }
};