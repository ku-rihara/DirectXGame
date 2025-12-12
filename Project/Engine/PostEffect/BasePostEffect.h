#pragma once

#include <d3d12.h>
#include <d3dcommon.h>
#include <dxcapi.h>
#include <string>
#include <wrl.h>
#include <wrl/client.h>

namespace KetaEngine {

class DirectXCommon;
class ViewProjection;

/// <summary>
/// ポストエフェクトの基底クラス
/// </summary>
class BasePostEffect {
protected:
    virtual void CreateGraphicsPipeline(); //< グラフィックスパイプライン作成
    virtual void CreateRootSignature(); //< ルートシグネチャ作成

public:
    BasePostEffect()          = default;
    virtual ~BasePostEffect() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="dxCommon">DirectXCommon</param>
    virtual void Init(DirectXCommon* dxCommon);

    /// <summary>
    /// 描画ステート設定
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    virtual void SetDrawState(ID3D12GraphicsCommandList* commandList);

    virtual void Draw([[maybe_unused]] ID3D12GraphicsCommandList* commandList) = 0; //< 描画処理
    virtual void CreateConstantBuffer()                                        = 0; //< 定数バッファ作成
    virtual void DebugParamImGui()                                             = 0; //< ImGuiデバッグパラメータ

protected:
    DirectXCommon* dxCommon_ = nullptr;
    const ViewProjection* viewProjection_;

    D3D12_STATIC_SAMPLER_DESC staticSamplers_[1];
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;

    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStates_;

    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob_;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob_;

    std::wstring psName_ = L"resources/Shader/PostEffect/Fullscreen.PS.hlsl";
    std::wstring vsName_ = L"resources/Shader/PostEffect/Fullscreen.VS.hlsl";

public:
    void SetViewProjection(const ViewProjection* viewProjection);
};

}; // KetaEngine
