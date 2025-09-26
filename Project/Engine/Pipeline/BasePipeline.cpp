#include "BasePipeline.h"
#include "Dx/DirectXCommon.h"
#include "Dx/DxCompiler.h"
#include "function/Log.h"
#include <cassert>


void BasePipeline::Init(DirectXCommon* dxCommon) {
    // 引数で受けとる
    dxCommon_ = dxCommon;
    // グラフィックスパイプラインの生成
    CreateGraphicsPipeline();
 }

void BasePipeline::SerializeAndCreateRootSignature(const D3D12_ROOT_SIGNATURE_DESC& desc) {
    HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);
    if (FAILED(hr)) {
        if (errorBlob_) {
            Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
        }
        assert(false);
        return;
    }

    hr = dxCommon_->GetDevice()->CreateRootSignature(
        0,
        signatureBlob_->GetBufferPointer(),
        signatureBlob_->GetBufferSize(),
        IID_PPV_ARGS(&rootSignature_));
    assert(SUCCEEDED(hr));
}

Microsoft::WRL::ComPtr<IDxcBlob> BasePipeline::CompileShader(const wchar_t* shaderPath, const wchar_t* target) {
    auto blob = dxCommon_->GetDxCompiler()->CompileShader(shaderPath, target);
    assert(blob != nullptr);
    return blob;
}
