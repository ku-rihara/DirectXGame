#pragma once

#include <d3d12.h>
#include <d3dcommon.h>
#include <dxcapi.h>
#include <wrl/client.h>

class DirectXCommon;
enum class BlendMode;

class BasePipeline {
public:
    BasePipeline()          = default;
    virtual ~BasePipeline() = default;

    virtual void Init(DirectXCommon* dxCommon);
    virtual void PreDraw(ID3D12GraphicsCommandList* commandList) = 0;

    // Blend Set
    virtual void PreBlendSet(ID3D12GraphicsCommandList* commandList, const BlendMode& blendMode) = 0;

protected:
    virtual void CreateRootSignature()    = 0;
    virtual void CreateGraphicsPipeline() = 0;

    void SerializeAndCreateRootSignature(const D3D12_ROOT_SIGNATURE_DESC& desc);
    Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(const wchar_t* shaderPath, const wchar_t* target);

    // Common member variables
    DirectXCommon* dxCommon_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;

    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob_ = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob_     = nullptr;

    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_ = nullptr;
    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_  = nullptr;

    D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};

private:
    // コピー禁止
    BasePipeline(const BasePipeline&)            = delete;
    BasePipeline& operator=(const BasePipeline&) = delete;

public:
    // Common getter methods
    virtual DirectXCommon* GetDxCommon() const { return dxCommon_; }
    virtual ID3D12RootSignature* GetRootSignature() const { return rootSignature_.Get(); }
};