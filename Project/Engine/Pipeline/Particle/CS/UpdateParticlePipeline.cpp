#include "UpdateParticlePipeline.h"
#include "Dx/DirectXCommon.h"
#include "Dx/DxCompiler.h"
#include "function/Log.h"
#include <cassert>
#include <string>

void UpdateParticlePipeline::Init(DirectXCommon* dxCommon) {
    BaseCSPipeline::Init(dxCommon);
}


void UpdateParticlePipeline::CreateRootSignature() {
    // DescriptorRangeを設定
    D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};

    // gParticles (u0) - RWStructuredBuffer
    descriptorRange[0].BaseShaderRegister                = 0;
    descriptorRange[0].NumDescriptors                    = 1;
    descriptorRange[0].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
    descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    // RootParameterを作成
    D3D12_ROOT_PARAMETER rootParameters[2] = {};

    // 0: gParticles (u0) - RWStructuredBuffer
    rootParameters[0].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[0].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;
    rootParameters[0].DescriptorTable.pDescriptorRanges   = &descriptorRange[0];
    rootParameters[0].DescriptorTable.NumDescriptorRanges = 1;

    // 1: perFrame (b0) - ConstantBuffer
    rootParameters[1].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[1].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;
    rootParameters[1].Descriptor.ShaderRegister = 0;

    D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
    descriptionRootSignature.Flags             = D3D12_ROOT_SIGNATURE_FLAG_NONE;
    descriptionRootSignature.pParameters       = rootParameters;
    descriptionRootSignature.NumParameters     = _countof(rootParameters);
    descriptionRootSignature.pStaticSamplers   = nullptr;
    descriptionRootSignature.NumStaticSamplers = 0;

    // シリアライズしてバイナリにする
    SerializeAndCreateRootSignature(descriptionRootSignature);
}

void UpdateParticlePipeline::CreateComputePipeline() {
    HRESULT hr = 0;

    // Compute Shaderをコンパイルする
    computeShaderBlob_ = dxCommon_->GetDxCompiler()->CompileShader(L"resources/Shader/Particle/CS/UpdateParticle.CS.hlsl", L"cs_6_0");
    assert(computeShaderBlob_ != nullptr);

    // Compute Pipeline State Descriptionを設定
    D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};
    computePipelineStateDesc.CS = {
        .pShaderBytecode = computeShaderBlob_->GetBufferPointer(),
        .BytecodeLength  = computeShaderBlob_->GetBufferSize(),
    };
    computePipelineStateDesc.pRootSignature = rootSignature_.Get();

    // Compute Pipeline Stateを作成
    hr = dxCommon_->GetDevice()->CreateComputePipelineState(&computePipelineStateDesc, IID_PPV_ARGS(&computePipelineState_));
    assert(SUCCEEDED(hr));
}

void UpdateParticlePipeline::PreDraw(ID3D12GraphicsCommandList* commandList) {
    // Compute用のRootSignatureを設定
    commandList->SetComputeRootSignature(rootSignature_.Get());
    // Pipeline Stateを設定
    commandList->SetPipelineState(computePipelineState_.Get());
}

void UpdateParticlePipeline::Dispatch(ID3D12GraphicsCommandList* commandList, const UINT& numVertices) {
    numVertices;
    commandList->Dispatch(1, 1, 1);
}