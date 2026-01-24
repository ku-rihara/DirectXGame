#include "EmitParticlePipeline.h"

using namespace KetaEngine;
#include "Base/Dx/DirectXCommon.h"
#include "Base/Dx/DxCompiler.h"
#include "function/Log.h"
#include <cassert>
#include <string>

void EmitParticlePipeline::Init(DirectXCommon* dxCommon) {
    BaseCSPipeline::Init(dxCommon);
}

void EmitParticlePipeline::CreateRootSignature() {
    // DescriptorRangeを設定
    D3D12_DESCRIPTOR_RANGE descriptorRange[3] = {};

    // gParticles (u0) - RWStructuredBuffer
    descriptorRange[0].BaseShaderRegister                = 0;
    descriptorRange[0].NumDescriptors                    = 1;
    descriptorRange[0].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
    descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    // gFreeListIndex (u1) - RWStructuredBuffer
    descriptorRange[1].BaseShaderRegister                = 1;
    descriptorRange[1].NumDescriptors                    = 1;
    descriptorRange[1].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
    descriptorRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    // gFreeList (u2) - RWStructuredBuffer
    descriptorRange[2].BaseShaderRegister                = 2;
    descriptorRange[2].NumDescriptors                    = 1;
    descriptorRange[2].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
    descriptorRange[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    // RootParameterを作成 
    D3D12_ROOT_PARAMETER rootParameters[8] = {};

    // 0: gEmitter (b0) - ConstantBuffer
    rootParameters[0].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[0].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;
    rootParameters[0].Descriptor.ShaderRegister = 0;

    // 1: gParticles (u0) - RWStructuredBuffer
    rootParameters[1].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[1].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;
    rootParameters[1].DescriptorTable.pDescriptorRanges   = &descriptorRange[0];
    rootParameters[1].DescriptorTable.NumDescriptorRanges = 1;

    // 2: gPerFrame (b1) - ConstantBuffer
    rootParameters[2].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[2].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;
    rootParameters[2].Descriptor.ShaderRegister = 1;

    // 3: gFreeListIndex (u1) - RWStructuredBuffer
    rootParameters[3].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[3].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;
    rootParameters[3].DescriptorTable.pDescriptorRanges   = &descriptorRange[1];
    rootParameters[3].DescriptorTable.NumDescriptorRanges = 1;

    // 4: gFreeList (u2) - RWStructuredBuffer
    rootParameters[4].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[4].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;
    rootParameters[4].DescriptorTable.pDescriptorRanges   = &descriptorRange[2];
    rootParameters[4].DescriptorTable.NumDescriptorRanges = 1;

    // 5: EmitTransformParams (b2) - ConstantBuffer
    rootParameters[5].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[5].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;
    rootParameters[5].Descriptor.ShaderRegister = 2;

    // 6: EmitPhysicsParams (b3) - ConstantBuffer
    rootParameters[6].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[6].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;
    rootParameters[6].Descriptor.ShaderRegister = 3;

    // 7: EmitAppearanceParams (b4) - ConstantBuffer
    rootParameters[7].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[7].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;
    rootParameters[7].Descriptor.ShaderRegister = 4;

    // Root Signature Descriptionを設定
    D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
    descriptionRootSignature.Flags             = D3D12_ROOT_SIGNATURE_FLAG_NONE;
    descriptionRootSignature.pParameters       = rootParameters;
    descriptionRootSignature.NumParameters     = _countof(rootParameters);
    descriptionRootSignature.pStaticSamplers   = nullptr;
    descriptionRootSignature.NumStaticSamplers = 0;

    // シリアライズしてバイナリにする
    SerializeAndCreateRootSignature(descriptionRootSignature);
}

void EmitParticlePipeline::CreateComputePipeline() {
    HRESULT hr = 0;

    // Compute Shaderをコンパイルする
    computeShaderBlob_ = dxCommon_->GetDxCompiler()->CompileShader(
        L"resources/Shader/Particle/CS/EmitParticle.CS.hlsl", L"cs_6_0");
    assert(computeShaderBlob_ != nullptr);

    // Compute Pipeline State Descriptionを設定
    D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};
    computePipelineStateDesc.CS = {
        .pShaderBytecode = computeShaderBlob_->GetBufferPointer(),
        .BytecodeLength  = computeShaderBlob_->GetBufferSize(),
    };
    computePipelineStateDesc.pRootSignature = rootSignature_.Get();

    // Compute Pipeline Stateを作成
    hr = dxCommon_->GetDevice()->CreateComputePipelineState(
        &computePipelineStateDesc, IID_PPV_ARGS(&computePipelineState_));
    assert(SUCCEEDED(hr));
}

void EmitParticlePipeline::PreDraw(ID3D12GraphicsCommandList* commandList) {
    // Compute用のRootSignatureを設定
    commandList->SetComputeRootSignature(rootSignature_.Get());
    // Pipeline Stateを設定
    commandList->SetPipelineState(computePipelineState_.Get());
}

void EmitParticlePipeline::Dispatch(ID3D12GraphicsCommandList* commandList, const UINT& numVertices) {
    numVertices;
    commandList->Dispatch(1, 1, 1);
}
