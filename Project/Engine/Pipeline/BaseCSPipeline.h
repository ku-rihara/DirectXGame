#pragma once
#include <d3d12.h>
#include <d3dcommon.h>
#include <dxcapi.h>
#include <Windows.h>
#include <wrl/client.h>

class DirectXCommon;
enum class BlendMode;

class BaseCSPipeline {
public:
    BaseCSPipeline()          = default;
    virtual ~BaseCSPipeline() = default;

    virtual void Init(DirectXCommon* dxCommon);
    virtual void PreDraw(ID3D12GraphicsCommandList* commandList);

    // Dispatch
    virtual void Dispatch(ID3D12GraphicsCommandList* commandList,const UINT& numThreadsX) = 0;

protected:
    virtual void CreateRootSignature()   = 0;
    virtual void CreateComputePipeline() = 0;

    void SerializeAndCreateRootSignature(const D3D12_ROOT_SIGNATURE_DESC& desc);

    // Common member variables
    DirectXCommon* dxCommon_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob_;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob_;
    Microsoft::WRL::ComPtr<IDxcBlob> computeShaderBlob_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> computePipelineState_;

private:
    // コピー禁止
    BaseCSPipeline(const BaseCSPipeline&)            = delete;
    BaseCSPipeline& operator=(const BaseCSPipeline&) = delete;

public:
    // getter
    DirectXCommon* GetDxCommon() const { return dxCommon_; }
    ID3D12RootSignature* GetRootSignature() const { return rootSignature_.Get(); }
    ID3D12PipelineState* GetComputePipelineState() const { return computePipelineState_.Get(); }
};