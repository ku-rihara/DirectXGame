#pragma once

#include <d3d12.h>
#include <d3dcommon.h>
#include <dxcapi.h>
#include <wrl/client.h>

class DirectXCommon;
enum class BlendMode;

/// <summary>
/// グラフィックスパイプラインの基底クラス
/// </summary>
class BasePipeline {
public:
    BasePipeline()          = default;
    virtual ~BasePipeline() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="dxCommon">DirectXCommon</param>
    virtual void Init(DirectXCommon* dxCommon);

    /// <summary>
    /// 描画前処理
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    virtual void PreDraw(ID3D12GraphicsCommandList* commandList) = 0;

    /// <summary>
    /// ブレンドモード設定
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    /// <param name="blendMode">ブレンドモード</param>
    virtual void PreBlendSet(ID3D12GraphicsCommandList* commandList, const BlendMode& blendMode) = 0;

protected:
    virtual void CreateRootSignature()    = 0; //< ルートシグネチャ作成
    virtual void CreateGraphicsPipeline() = 0; //< グラフィックスパイプライン作成

    /// <summary>
    /// ルートシグネチャのシリアライズと作成
    /// </summary>
    /// <param name="desc">ルートシグネチャの設定</param>
    void SerializeAndCreateRootSignature(const D3D12_ROOT_SIGNATURE_DESC& desc);

    /// <summary>
    /// シェーダーのコンパイル
    /// </summary>
    /// <param name="shaderPath">シェーダーファイルパス</param>
    /// <param name="target">シェーダーターゲット</param>
    /// <returns>コンパイル済みシェーダー</returns>
    Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(const wchar_t* shaderPath, const wchar_t* target);

    DirectXCommon* dxCommon_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;

    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob_ = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob_     = nullptr;

    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_ = nullptr;
    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_  = nullptr;

    D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};

private:
    BasePipeline(const BasePipeline&)            = delete;
    BasePipeline& operator=(const BasePipeline&) = delete;

public:
    virtual DirectXCommon* GetDxCommon() const { return dxCommon_; }
    virtual ID3D12RootSignature* GetRootSignature() const { return rootSignature_.Get(); }
};