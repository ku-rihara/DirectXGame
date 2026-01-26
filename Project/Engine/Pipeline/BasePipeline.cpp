#include "BasePipeline.h"

using namespace KetaEngine;
#include "Base/Dx/DirectXCommon.h"
#include "Base/Dx/DxCompiler.h"
#include "function/Log.h"
#include <cassert>

void BasePipeline::Init(DirectXCommon* dxCommon) {
   
    dxCommon_ = dxCommon;

    // パイプラインの生成、ルートシグネチャの生成
    CreateRootSignature();
    CreateGraphicsPipeline();
}

void BasePipeline::SerializeAndCreateRootSignature(const D3D12_ROOT_SIGNATURE_DESC& desc) {
    // シリアライズしてバイナリにする
    HRESULT hr     = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);
    if (FAILED(hr)) {
        Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
        assert(false);
    }
    // バイナリを元に生成
    rootSignature_ = nullptr;
    hr             = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
    assert(SUCCEEDED(hr));
}

Microsoft::WRL::ComPtr<IDxcBlob> BasePipeline::CompileShader(const wchar_t* shaderPath, const wchar_t* target) {
    auto blob = dxCommon_->GetDxCompiler()->CompileShader(shaderPath, target);
    assert(blob != nullptr);
    return blob;
}
