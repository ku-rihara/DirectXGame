#include "RibbonTrailDistortionPipeline.h"

using namespace KetaEngine;
#include "Base/Dx/DirectXCommon.h"
#include <cassert>
#include <d3dx12.h>
#include <stdexcept>

///============================================================
/// 初期化
///============================================================
void RibbonTrailDistortionPipeline::Init(DirectXCommon* dxCommon) {
    dxCommon_ = dxCommon;
    CreateRootSignature();
    CreateGraphicsPipeline();
}

///============================================================
/// ルートシグネチャ作成
///============================================================
void RibbonTrailDistortionPipeline::CreateRootSignature() {
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

    // ノイズテクスチャ用 DescriptorRange (t0, PS)
    D3D12_DESCRIPTOR_RANGE distortionMapRange{};
    distortionMapRange.RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    distortionMapRange.NumDescriptors                    = 1;
    distortionMapRange.BaseShaderRegister                = 0;
    distortionMapRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    D3D12_ROOT_PARAMETER rootParameters[static_cast<UINT>(RibbonTrailDistortionRootParameter::Count)] = {};

    // TransformationMatrix (b0, VS)
    rootParameters[static_cast<UINT>(RibbonTrailDistortionRootParameter::TransformationMatrix)].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[static_cast<UINT>(RibbonTrailDistortionRootParameter::TransformationMatrix)].ShaderVisibility          = D3D12_SHADER_VISIBILITY_VERTEX;
    rootParameters[static_cast<UINT>(RibbonTrailDistortionRootParameter::TransformationMatrix)].Descriptor.ShaderRegister = 0;

    // DistortionUVScroll (b1, VS)
    rootParameters[static_cast<UINT>(RibbonTrailDistortionRootParameter::DistortionUVScroll)].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[static_cast<UINT>(RibbonTrailDistortionRootParameter::DistortionUVScroll)].ShaderVisibility          = D3D12_SHADER_VISIBILITY_VERTEX;
    rootParameters[static_cast<UINT>(RibbonTrailDistortionRootParameter::DistortionUVScroll)].Descriptor.ShaderRegister = 1;

    // DistortionParam (b1, PS) ※VS の b1 と別シェーダーステージなので競合しない
    rootParameters[static_cast<UINT>(RibbonTrailDistortionRootParameter::DistortionParam)].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[static_cast<UINT>(RibbonTrailDistortionRootParameter::DistortionParam)].ShaderVisibility          = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[static_cast<UINT>(RibbonTrailDistortionRootParameter::DistortionParam)].Descriptor.ShaderRegister = 1;

    // DistortionMap (t0, PS)
    rootParameters[static_cast<UINT>(RibbonTrailDistortionRootParameter::DistortionMap)].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[static_cast<UINT>(RibbonTrailDistortionRootParameter::DistortionMap)].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[static_cast<UINT>(RibbonTrailDistortionRootParameter::DistortionMap)].DescriptorTable.pDescriptorRanges   = &distortionMapRange;
    rootParameters[static_cast<UINT>(RibbonTrailDistortionRootParameter::DistortionMap)].DescriptorTable.NumDescriptorRanges = 1;

    D3D12_ROOT_SIGNATURE_DESC desc{};
    desc.Flags             = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    desc.pParameters       = rootParameters;
    desc.NumParameters     = static_cast<UINT>(RibbonTrailDistortionRootParameter::Count);
    desc.pStaticSamplers   = &sampler;
    desc.NumStaticSamplers = 1;

    SerializeAndCreateRootSignature(desc);
}

///============================================================
/// グラフィックスパイプライン作成
///============================================================
void RibbonTrailDistortionPipeline::CreateGraphicsPipeline() {
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

    // シェーダーコンパイル
    vertexShaderBlob_ = CompileShader(L"resources/Shader/RibbonTrailDistortion.VS.hlsl", L"vs_6_0");
    pixelShaderBlob_  = CompileShader(L"resources/Shader/RibbonTrailDistortion.PS.hlsl", L"ps_6_0");

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
    desc.RTVFormats[0]         = DXGI_FORMAT_R16G16_FLOAT; // UVオフセット(XY)格納用
    desc.DSVFormat             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    desc.SampleDesc.Count      = 1;
    desc.SampleMask            = D3D12_DEFAULT_SAMPLE_MASK;

    HRESULT hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&graphicsPipelineState_));
    if (FAILED(hr)) {
        throw std::runtime_error("RibbonTrailDistortionPipeline: CreateGraphicsPipelineState failed");
    }
}

///============================================================
/// 描画前処理
///============================================================
void RibbonTrailDistortionPipeline::PreDraw(ID3D12GraphicsCommandList* commandList) {
    commandList->SetPipelineState(graphicsPipelineState_.Get());
    commandList->SetGraphicsRootSignature(rootSignature_.Get());
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void RibbonTrailDistortionPipeline::PreBlendSet(ID3D12GraphicsCommandList* commandList, const BlendMode& blendMode) {
    commandList;
    blendMode;
}
