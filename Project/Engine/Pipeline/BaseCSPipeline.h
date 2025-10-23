#pragma once
#include <d3d12.h>
#include <d3dcommon.h>
#include <dxcapi.h>
#include <Windows.h>
#include <wrl/client.h>

class DirectXCommon;
enum class BlendMode;

/// <summary>
/// コンピュートシェーダパイプラインの基底クラス
/// </summary>
class BaseCSPipeline {
public:
    BaseCSPipeline()          = default;
    virtual ~BaseCSPipeline() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="dxCommon">DirectXCommon</param>
    virtual void Init(DirectXCommon* dxCommon);

    /// <summary>
    /// 描画前処理
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    virtual void PreDraw(ID3D12GraphicsCommandList* commandList);

    /// <summary>
    /// ディスパッチ実行
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    /// <param name="numThreadsX">スレッド数X</param>
    virtual void Dispatch(ID3D12GraphicsCommandList* commandList, const UINT& numThreadsX) = 0;

protected:
    virtual void CreateRootSignature()   = 0; //< ルートシグネチャ作成
    virtual void CreateComputePipeline() = 0; //< コンピュートパイプライン作成

    /// <summary>
    /// ルートシグネチャのシリアライズと作成
    /// </summary>
    /// <param name="desc">ルートシグネチャの設定</param>
    void SerializeAndCreateRootSignature(const D3D12_ROOT_SIGNATURE_DESC& desc);

    DirectXCommon* dxCommon_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob_;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob_;
    Microsoft::WRL::ComPtr<IDxcBlob> computeShaderBlob_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> computePipelineState_;

private:
    BaseCSPipeline(const BaseCSPipeline&)            = delete;
    BaseCSPipeline& operator=(const BaseCSPipeline&) = delete;

public:
    DirectXCommon* GetDxCommon() const { return dxCommon_; }
    ID3D12RootSignature* GetRootSignature() const { return rootSignature_.Get(); }
    ID3D12PipelineState* GetComputePipelineState() const { return computePipelineState_.Get(); }
};