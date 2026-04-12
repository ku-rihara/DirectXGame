#include "DistortionPipeline.h"

using namespace KetaEngine;

#include "Base/Dx/DirectXCommon.h"
#include <cassert>
#include <d3dx12.h>
#include <stdexcept>

///============================================================
/// コンストラクタ
///============================================================
DistortionPipeline::DistortionPipeline(DistortionMode mode)
    : mode_(mode) {}

///============================================================
/// 初期化
///============================================================
void DistortionPipeline::Init(DirectXCommon* dxCommon) {
    dxCommon_ = dxCommon;
    CreateRootSignature();
    CreateGraphicsPipeline();
}

///============================================================
/// ルートシグネチャ作成（モード別に委譲）
///============================================================
void DistortionPipeline::CreateRootSignature() {
    if (mode_ == DistortionMode::Ribbon) {
        CreateRibbonRootSignature();
    } else {
        CreateParticleRootSignature();
    }
}

///============================================================
/// グラフィックスパイプライン作成（モード別に委譲）
///============================================================
void DistortionPipeline::CreateGraphicsPipeline() {
    if (mode_ == DistortionMode::Ribbon) {
        CreateRibbonPipeline();
    } else {
        CreateParticlePipeline();
    }
}

///============================================================
/// 描画前処理
///============================================================
void DistortionPipeline::PreDraw(ID3D12GraphicsCommandList* commandList) {
    commandList->SetPipelineState(pso_.Get());
    commandList->SetGraphicsRootSignature(rootSignature_.Get());
    if (mode_ == DistortionMode::Ribbon) {
        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    } else {
        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }
}

void DistortionPipeline::PreBlendSet(ID3D12GraphicsCommandList* commandList, const BlendMode& blendMode) {
    commandList;
    blendMode;
}

///============================================================
/// RibbonTrail 用ルートシグネチャ
///============================================================
void DistortionPipeline::CreateRibbonRootSignature() {
    // サンプラー（ノイズテクスチャ用）
    D3D12_STATIC_SAMPLER_DESC sampler{};
    sampler.Filter           = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    sampler.AddressU         = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.AddressV         = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.AddressW         = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER;
    sampler.MaxLOD           = D3D12_FLOAT32_MAX;
    sampler.ShaderRegister   = 0;
    sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    // DistortionMap (t0, PS) 用 DescriptorRange
    D3D12_DESCRIPTOR_RANGE distortionMapRange{};
    distortionMapRange.RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    distortionMapRange.NumDescriptors                    = 1;
    distortionMapRange.BaseShaderRegister                = 0;
    distortionMapRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    D3D12_ROOT_PARAMETER rootParameters[static_cast<UINT>(DistortionRibbonRootParam::Count)] = {};

    // TransformationMatrix (b0, VS)
    rootParameters[static_cast<UINT>(DistortionRibbonRootParam::TransformationMatrix)].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[static_cast<UINT>(DistortionRibbonRootParam::TransformationMatrix)].ShaderVisibility          = D3D12_SHADER_VISIBILITY_VERTEX;
    rootParameters[static_cast<UINT>(DistortionRibbonRootParam::TransformationMatrix)].Descriptor.ShaderRegister = 0;

    // DistortionUVScroll (b1, VS)
    rootParameters[static_cast<UINT>(DistortionRibbonRootParam::DistortionUVScroll)].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[static_cast<UINT>(DistortionRibbonRootParam::DistortionUVScroll)].ShaderVisibility          = D3D12_SHADER_VISIBILITY_VERTEX;
    rootParameters[static_cast<UINT>(DistortionRibbonRootParam::DistortionUVScroll)].Descriptor.ShaderRegister = 1;

    // DistortionParam (b0, PS) - 32ビット定数 (strength + padding x3)
    rootParameters[static_cast<UINT>(DistortionRibbonRootParam::DistortionParam)].ParameterType            = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    rootParameters[static_cast<UINT>(DistortionRibbonRootParam::DistortionParam)].ShaderVisibility         = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[static_cast<UINT>(DistortionRibbonRootParam::DistortionParam)].Constants.ShaderRegister = 0;
    rootParameters[static_cast<UINT>(DistortionRibbonRootParam::DistortionParam)].Constants.RegisterSpace  = 0;
    rootParameters[static_cast<UINT>(DistortionRibbonRootParam::DistortionParam)].Constants.Num32BitValues = 4;

    // DistortionMap (t0, PS)
    rootParameters[static_cast<UINT>(DistortionRibbonRootParam::DistortionMap)].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[static_cast<UINT>(DistortionRibbonRootParam::DistortionMap)].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[static_cast<UINT>(DistortionRibbonRootParam::DistortionMap)].DescriptorTable.pDescriptorRanges   = &distortionMapRange;
    rootParameters[static_cast<UINT>(DistortionRibbonRootParam::DistortionMap)].DescriptorTable.NumDescriptorRanges = 1;

    D3D12_ROOT_SIGNATURE_DESC desc{};
    desc.Flags             = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    desc.pParameters       = rootParameters;
    desc.NumParameters     = static_cast<UINT>(DistortionRibbonRootParam::Count);
    desc.pStaticSamplers   = &sampler;
    desc.NumStaticSamplers = 1;

    SerializeAndCreateRootSignature(desc);
}

///============================================================
/// CPUParticle 用ルートシグネチャ
///============================================================
void DistortionPipeline::CreateParticleRootSignature() {
    // サンプラー（ノイズテクスチャ用）
    D3D12_STATIC_SAMPLER_DESC sampler{};
    sampler.Filter           = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    sampler.AddressU         = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.AddressV         = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.AddressW         = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER;
    sampler.MaxLOD           = D3D12_FLOAT32_MAX;
    sampler.ShaderRegister   = 0;
    sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    // ParticleData (t0, VS) 用 DescriptorRange
    D3D12_DESCRIPTOR_RANGE particleDataRange{};
    particleDataRange.RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    particleDataRange.NumDescriptors                    = 1;
    particleDataRange.BaseShaderRegister                = 0;
    particleDataRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    // DistortionMap (t0, PS) 用 DescriptorRange
    D3D12_DESCRIPTOR_RANGE distortionMapRange{};
    distortionMapRange.RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    distortionMapRange.NumDescriptors                    = 1;
    distortionMapRange.BaseShaderRegister                = 0;
    distortionMapRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    D3D12_ROOT_PARAMETER rootParameters[static_cast<UINT>(DistortionParticleRootParam::Count)] = {};

    // ParticleData (t0, VS)
    rootParameters[static_cast<UINT>(DistortionParticleRootParam::ParticleData)].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[static_cast<UINT>(DistortionParticleRootParam::ParticleData)].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_VERTEX;
    rootParameters[static_cast<UINT>(DistortionParticleRootParam::ParticleData)].DescriptorTable.pDescriptorRanges   = &particleDataRange;
    rootParameters[static_cast<UINT>(DistortionParticleRootParam::ParticleData)].DescriptorTable.NumDescriptorRanges = 1;

    // DistortionParam (b0, PS) - 32ビット定数
    rootParameters[static_cast<UINT>(DistortionParticleRootParam::DistortionParam)].ParameterType            = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    rootParameters[static_cast<UINT>(DistortionParticleRootParam::DistortionParam)].ShaderVisibility         = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[static_cast<UINT>(DistortionParticleRootParam::DistortionParam)].Constants.ShaderRegister = 0;
    rootParameters[static_cast<UINT>(DistortionParticleRootParam::DistortionParam)].Constants.RegisterSpace  = 0;
    rootParameters[static_cast<UINT>(DistortionParticleRootParam::DistortionParam)].Constants.Num32BitValues = 4;

    // DistortionMap (t0, PS)
    rootParameters[static_cast<UINT>(DistortionParticleRootParam::DistortionMap)].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[static_cast<UINT>(DistortionParticleRootParam::DistortionMap)].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[static_cast<UINT>(DistortionParticleRootParam::DistortionMap)].DescriptorTable.pDescriptorRanges   = &distortionMapRange;
    rootParameters[static_cast<UINT>(DistortionParticleRootParam::DistortionMap)].DescriptorTable.NumDescriptorRanges = 1;

    D3D12_ROOT_SIGNATURE_DESC desc{};
    desc.Flags             = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    desc.pParameters       = rootParameters;
    desc.NumParameters     = static_cast<UINT>(DistortionParticleRootParam::Count);
    desc.pStaticSamplers   = &sampler;
    desc.NumStaticSamplers = 1;

    SerializeAndCreateRootSignature(desc);
}

///============================================================
/// RibbonTrail 用 PSO
///============================================================
void DistortionPipeline::CreateRibbonPipeline() {
    // 入力レイアウト（RibbonVertex: POSITION / COLOR / TEXCOORD）
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
    inputElementDescs[0].SemanticName      = "POSITION";
    inputElementDescs[0].SemanticIndex     = 0;
    inputElementDescs[0].Format            = DXGI_FORMAT_R32G32B32_FLOAT;
    inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    inputElementDescs[1].SemanticName      = "COLOR";
    inputElementDescs[1].SemanticIndex     = 0;
    inputElementDescs[1].Format            = DXGI_FORMAT_R32G32B32A32_FLOAT;
    inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    inputElementDescs[2].SemanticName      = "TEXCOORD";
    inputElementDescs[2].SemanticIndex     = 0;
    inputElementDescs[2].Format            = DXGI_FORMAT_R32G32_FLOAT;
    inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
    inputLayoutDesc.pInputElementDescs = inputElementDescs;
    inputLayoutDesc.NumElements        = _countof(inputElementDescs);

    // シェーダーコンパイル（共通PS）
    vertexShaderBlob_ = CompileShader(L"resources/Shader/Distortion/Distortion.Ribbon.VS.hlsl", L"vs_6_0");
    pixelShaderBlob_  = CompileShader(L"resources/Shader/Distortion/Distortion.PS.hlsl", L"ps_6_0");

    // ブレンド（加算合成）
    D3D12_BLEND_DESC blendDesc{};
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_RED | D3D12_COLOR_WRITE_ENABLE_GREEN;
    blendDesc.RenderTarget[0].BlendEnable           = TRUE;
    blendDesc.RenderTarget[0].SrcBlend              = D3D12_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp               = D3D12_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].DestBlend             = D3D12_BLEND_ONE;
    blendDesc.RenderTarget[0].SrcBlendAlpha         = D3D12_BLEND_ONE;
    blendDesc.RenderTarget[0].BlendOpAlpha          = D3D12_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].DestBlendAlpha        = D3D12_BLEND_ZERO;

    // ラスタライザー（両面描画）
    D3D12_RASTERIZER_DESC rasterizerDesc{};
    rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
    rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

    // 深度テスト（読み取りのみ）
    depthStencilDesc_.DepthEnable    = true;
    depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc_.DepthFunc      = D3D12_COMPARISON_FUNC_LESS_EQUAL;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};
    desc.InputLayout           = inputLayoutDesc;
    desc.pRootSignature        = rootSignature_.Get();
    desc.VS                    = {vertexShaderBlob_->GetBufferPointer(), vertexShaderBlob_->GetBufferSize()};
    desc.PS                    = {pixelShaderBlob_->GetBufferPointer(), pixelShaderBlob_->GetBufferSize()};
    desc.BlendState            = blendDesc;
    desc.RasterizerState       = rasterizerDesc;
    desc.DepthStencilState     = depthStencilDesc_;
    desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    desc.NumRenderTargets      = 1;
    desc.RTVFormats[0]         = DXGI_FORMAT_R16G16_FLOAT;
    desc.DSVFormat             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    desc.SampleDesc.Count      = 1;
    desc.SampleMask            = D3D12_DEFAULT_SAMPLE_MASK;

    HRESULT hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pso_));
    if (FAILED(hr)) {
        throw std::runtime_error("DistortionPipeline(Ribbon): CreateGraphicsPipelineState failed");
    }
}

///============================================================
/// CPUParticle 用 PSO
///============================================================
void DistortionPipeline::CreateParticlePipeline() {
    // 入力レイアウト（ParticleVertex: POSITION / TEXCOORD / NORMAL）
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
    inputElementDescs[0].SemanticName      = "POSITION";
    inputElementDescs[0].SemanticIndex     = 0;
    inputElementDescs[0].Format            = DXGI_FORMAT_R32G32B32A32_FLOAT;
    inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    inputElementDescs[1].SemanticName      = "TEXCOORD";
    inputElementDescs[1].SemanticIndex     = 0;
    inputElementDescs[1].Format            = DXGI_FORMAT_R32G32_FLOAT;
    inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    inputElementDescs[2].SemanticName      = "NORMAL";
    inputElementDescs[2].SemanticIndex     = 0;
    inputElementDescs[2].Format            = DXGI_FORMAT_R32G32B32_FLOAT;
    inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
    inputLayoutDesc.pInputElementDescs = inputElementDescs;
    inputLayoutDesc.NumElements        = _countof(inputElementDescs);

    // シェーダーコンパイル（共通PS）
    vertexShaderBlob_ = CompileShader(L"resources/Shader/Distortion/Distortion.Particle.VS.hlsl", L"vs_6_0");
    pixelShaderBlob_  = CompileShader(L"resources/Shader/Distortion/Distortion.PS.hlsl", L"ps_6_0");

    // ブレンド（加算合成 - Ribbon と同一）
    D3D12_BLEND_DESC blendDesc{};
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_RED | D3D12_COLOR_WRITE_ENABLE_GREEN;
    blendDesc.RenderTarget[0].BlendEnable           = TRUE;
    blendDesc.RenderTarget[0].SrcBlend              = D3D12_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp               = D3D12_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].DestBlend             = D3D12_BLEND_ONE;
    blendDesc.RenderTarget[0].SrcBlendAlpha         = D3D12_BLEND_ONE;
    blendDesc.RenderTarget[0].BlendOpAlpha          = D3D12_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].DestBlendAlpha        = D3D12_BLEND_ZERO;

    // ラスタライザー（両面描画）
    D3D12_RASTERIZER_DESC rasterizerDesc{};
    rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
    rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

    // 深度テスト（読み取りのみ）
    depthStencilDesc_.DepthEnable    = true;
    depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc_.DepthFunc      = D3D12_COMPARISON_FUNC_LESS_EQUAL;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};
    desc.InputLayout           = inputLayoutDesc;
    desc.pRootSignature        = rootSignature_.Get();
    desc.VS                    = {vertexShaderBlob_->GetBufferPointer(), vertexShaderBlob_->GetBufferSize()};
    desc.PS                    = {pixelShaderBlob_->GetBufferPointer(), pixelShaderBlob_->GetBufferSize()};
    desc.BlendState            = blendDesc;
    desc.RasterizerState       = rasterizerDesc;
    desc.DepthStencilState     = depthStencilDesc_;
    desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    desc.NumRenderTargets      = 1;
    desc.RTVFormats[0]         = DXGI_FORMAT_R16G16_FLOAT;
    desc.DSVFormat             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    desc.SampleDesc.Count      = 1;
    desc.SampleMask            = D3D12_DEFAULT_SAMPLE_MASK;

    HRESULT hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pso_));
    if (FAILED(hr)) {
        throw std::runtime_error("DistortionPipeline(Particle): CreateGraphicsPipelineState failed");
    }
}
