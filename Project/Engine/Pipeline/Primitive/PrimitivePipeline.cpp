#include "PrimitivePipeline.h"
#include "Base/Dx/DirectXCommon.h"
#include "Base/Dx/DxCompiler.h"
#include "Base/Material/ModelMaterial.h"
#include "function/Log.h"
#include <cassert>

using namespace KetaEngine;

void PrimitivePipeline::Init(DirectXCommon* dxCommon) {
    BasePipeline::Init(dxCommon);
}

void PrimitivePipeline::CreateRootSignature() {
    staticSampler_.Filter           = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    staticSampler_.AddressU         = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSampler_.AddressV         = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSampler_.AddressW         = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSampler_.ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER;
    staticSampler_.MaxLOD           = D3D12_FLOAT32_MAX;
    staticSampler_.ShaderRegister   = 0;
    staticSampler_.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    D3D12_DESCRIPTOR_RANGE texRange{};
    texRange.BaseShaderRegister                = 0;
    texRange.NumDescriptors                    = 1;
    texRange.RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    texRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    D3D12_ROOT_PARAMETER rootParams[static_cast<UINT>(PrimitiveRootParameter::Count)] = {};

    // Material (b0, Pixel)
    rootParams[static_cast<UINT>(PrimitiveRootParameter::Material)].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParams[static_cast<UINT>(PrimitiveRootParameter::Material)].ShaderVisibility          = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParams[static_cast<UINT>(PrimitiveRootParameter::Material)].Descriptor.ShaderRegister = 0;

    // TransformationMatrix (b0, Vertex)
    rootParams[static_cast<UINT>(PrimitiveRootParameter::TransformationMatrix)].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParams[static_cast<UINT>(PrimitiveRootParameter::TransformationMatrix)].ShaderVisibility          = D3D12_SHADER_VISIBILITY_VERTEX;
    rootParams[static_cast<UINT>(PrimitiveRootParameter::TransformationMatrix)].Descriptor.ShaderRegister = 0;

    // Texture2D (t0, Pixel)
    rootParams[static_cast<UINT>(PrimitiveRootParameter::Texture2D)].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParams[static_cast<UINT>(PrimitiveRootParameter::Texture2D)].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParams[static_cast<UINT>(PrimitiveRootParameter::Texture2D)].DescriptorTable.pDescriptorRanges   = &texRange;
    rootParams[static_cast<UINT>(PrimitiveRootParameter::Texture2D)].DescriptorTable.NumDescriptorRanges = 1;

    D3D12_ROOT_SIGNATURE_DESC desc{};
    desc.Flags             = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    desc.pParameters       = rootParams;
    desc.NumParameters     = static_cast<UINT>(PrimitiveRootParameter::Count);
    desc.pStaticSamplers   = &staticSampler_;
    desc.NumStaticSamplers = 1;

    SerializeAndCreateRootSignature(desc);
}

void PrimitivePipeline::CreateGraphicsPipeline() {
    HRESULT hr = 0;

    D3D12_INPUT_ELEMENT_DESC inputDescs[4] = {};
    inputDescs[0].SemanticName      = "POSITION";
    inputDescs[0].SemanticIndex     = 0;
    inputDescs[0].Format            = DXGI_FORMAT_R32G32B32A32_FLOAT;
    inputDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    inputDescs[1].SemanticName      = "TEXCOORD";
    inputDescs[1].SemanticIndex     = 0;
    inputDescs[1].Format            = DXGI_FORMAT_R32G32_FLOAT;
    inputDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    inputDescs[2].SemanticName      = "NORMAL";
    inputDescs[2].SemanticIndex     = 0;
    inputDescs[2].Format            = DXGI_FORMAT_R32G32B32_FLOAT;
    inputDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    inputDescs[3].SemanticName      = "COLOR";
    inputDescs[3].SemanticIndex     = 0;
    inputDescs[3].Format            = DXGI_FORMAT_R32G32B32A32_FLOAT;
    inputDescs[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    D3D12_INPUT_LAYOUT_DESC inputLayout{};
    inputLayout.pInputElementDescs = inputDescs;
    inputLayout.NumElements        = _countof(inputDescs);

    D3D12_BLEND_DESC blendAdd                      = {};
    blendAdd.RenderTarget[0].BlendEnable           = TRUE;
    blendAdd.RenderTarget[0].SrcBlend              = D3D12_BLEND_SRC_ALPHA;
    blendAdd.RenderTarget[0].DestBlend             = D3D12_BLEND_ONE;
    blendAdd.RenderTarget[0].BlendOp               = D3D12_BLEND_OP_ADD;
    blendAdd.RenderTarget[0].SrcBlendAlpha         = D3D12_BLEND_ONE;
    blendAdd.RenderTarget[0].DestBlendAlpha        = D3D12_BLEND_ZERO;
    blendAdd.RenderTarget[0].BlendOpAlpha          = D3D12_BLEND_OP_ADD;
    blendAdd.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    D3D12_BLEND_DESC blendNone                      = {};
    blendNone.RenderTarget[0].BlendEnable           = TRUE;
    blendNone.RenderTarget[0].SrcBlend              = D3D12_BLEND_SRC_ALPHA;
    blendNone.RenderTarget[0].DestBlend             = D3D12_BLEND_INV_SRC_ALPHA;
    blendNone.RenderTarget[0].BlendOp               = D3D12_BLEND_OP_ADD;
    blendNone.RenderTarget[0].SrcBlendAlpha         = D3D12_BLEND_ONE;
    blendNone.RenderTarget[0].DestBlendAlpha        = D3D12_BLEND_ZERO;
    blendNone.RenderTarget[0].BlendOpAlpha          = D3D12_BLEND_OP_ADD;
    blendNone.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    D3D12_RASTERIZER_DESC rasterDesc{};
    rasterDesc.CullMode = D3D12_CULL_MODE_NONE; // プリミティブは両面描画
    rasterDesc.FillMode = D3D12_FILL_MODE_SOLID;

    depthStencilDesc_.DepthEnable    = true;
    depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc_.DepthFunc      = D3D12_COMPARISON_FUNC_LESS_EQUAL;

    vertexShaderBlob_ = CompileShader(L"resources/Shader/Primitive.VS.hlsl", L"vs_6_0");
    pixelShaderBlob_  = CompileShader(L"resources/Shader/Primitive.PS.hlsl", L"ps_6_0");

    auto CreatePSO = [&](D3D12_BLEND_DESC& blend, Microsoft::WRL::ComPtr<ID3D12PipelineState>& pso) {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
        desc.pRootSignature        = rootSignature_.Get();
        desc.InputLayout           = inputLayout;
        desc.VS                    = {vertexShaderBlob_->GetBufferPointer(), vertexShaderBlob_->GetBufferSize()};
        desc.PS                    = {pixelShaderBlob_->GetBufferPointer(), pixelShaderBlob_->GetBufferSize()};
        desc.BlendState            = blend;
        desc.RasterizerState       = rasterDesc;
        desc.NumRenderTargets      = 1;
        desc.RTVFormats[0]         = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        desc.SampleDesc.Count      = 1;
        desc.SampleMask            = D3D12_DEFAULT_SAMPLE_MASK;
        desc.DepthStencilState     = depthStencilDesc_;
        desc.DSVFormat             = DXGI_FORMAT_D24_UNORM_S8_UINT;
        hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pso));
        assert(SUCCEEDED(hr));
    };

    CreatePSO(blendAdd,  psoAdd_);
    CreatePSO(blendNone, psoNone_);
}

void PrimitivePipeline::PreDraw(ID3D12GraphicsCommandList* commandList) {
    commandList->SetGraphicsRootSignature(rootSignature_.Get());
}

void PrimitivePipeline::PreBlendSet(ID3D12GraphicsCommandList* commandList, const BlendMode& blendMode) {
    switch (blendMode) {
    case BlendMode::None:
        commandList->SetPipelineState(psoNone_.Get());
        break;
    case BlendMode::Add:
        commandList->SetPipelineState(psoAdd_.Get());
        break;
    }
}
