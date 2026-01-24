#include "BaseCSPipeline.h"

using namespace KetaEngine;
#include "Base/Dx/DirectXCommon.h"
#include "Base/Dx/DxCompiler.h"
#include "function/Log.h"
#include <cassert>


void BaseCSPipeline::Init(DirectXCommon* dxCommon) {
    // 引数で受けとる
    dxCommon_ = dxCommon;
    // グラフィックスパイプラインの生成
    CreateRootSignature();
    CreateComputePipeline();
 }

void BaseCSPipeline::SerializeAndCreateRootSignature(const D3D12_ROOT_SIGNATURE_DESC& desc) {
     // シリアライズしてバイナリにする
     HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);
     if (FAILED(hr)) {
         Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
         assert(false);
     }
     // バイナリを元に生成
     rootSignature_ = nullptr;
     hr             = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
     assert(SUCCEEDED(hr));
 }


void BaseCSPipeline::PreDraw(ID3D12GraphicsCommandList* commandList) {
    // Compute用のRootSignatureを設定
    commandList->SetComputeRootSignature(rootSignature_.Get());
    // Pipeline Stateを設定
    commandList->SetPipelineState(computePipelineState_.Get());
}
