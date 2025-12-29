#include "SkyBoxPipeline.h"

using namespace KetaEngine;
#include "Dx/DxCompiler.h"
#include "function/Log.h"
#include "Material/ModelMaterial.h"
#include "base/TextureManager.h"
#include <cassert>
#include <string>

void SkyBoxPipeline::Init(DirectXCommon* dxCommon) {
    BasePipeline::Init(dxCommon);
    environmentalMapTextureHandle_ = TextureManager::GetInstance()->LoadTexture("Resources/Texture/output.dds");
}

void SkyBoxPipeline::CreateRootSignature() {

    
    staticSamplers_[0].Filter           = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイリニアフィルタ
    staticSamplers_[0].AddressU         = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0~1の範囲外をリピート
    staticSamplers_[0].AddressV         = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers_[0].AddressW         = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers_[0].ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
    staticSamplers_[0].MaxLOD           = D3D12_FLOAT32_MAX; // ありったけのMipMapを使う
    staticSamplers_[0].ShaderRegister   = 0; // レジスタ番号０
    staticSamplers_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // pxelShaderで使う

    // SRV (TextureCube) 用の descriptor range
    D3D12_DESCRIPTOR_RANGE descriptorRange{};
    descriptorRange.BaseShaderRegister                = 0; // t0
    descriptorRange.NumDescriptors                    = 1;
    descriptorRange.RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    // RootParameters
    D3D12_ROOT_PARAMETER rootParameters[3] = {};

    // Material（PixelShader）
    rootParameters[0].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[0].ShaderVisibility          = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[0].Descriptor.ShaderRegister = 0;

    // TransformationMatrix（VertexShader）
    rootParameters[1].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[1].ShaderVisibility          = D3D12_SHADER_VISIBILITY_VERTEX;
    rootParameters[1].Descriptor.ShaderRegister = 0;

    // TextureCube（PixelShader）
    rootParameters[2].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[2].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[2].DescriptorTable.pDescriptorRanges   = &descriptorRange;
    rootParameters[2].DescriptorTable.NumDescriptorRanges = 1;

    // Root Signature Description
    D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
    descriptionRootSignature.Flags             = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    descriptionRootSignature.pParameters       = rootParameters;
    descriptionRootSignature.NumParameters     = _countof(rootParameters);
    descriptionRootSignature.pStaticSamplers   = staticSamplers_;
    descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers_);

    // シリアライズしてバイナリにする
    SerializeAndCreateRootSignature(descriptionRootSignature);
}

void SkyBoxPipeline::CreateGraphicsPipeline() {

    HRESULT hr = 0;

    // InputLayoutの設定を行う
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

    // BlendStateの設定
    D3D12_BLEND_DESC blendDesc{};
    // すべての色要素を書き込む
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    blendDesc.RenderTarget[0].BlendEnable           = TRUE;
    blendDesc.RenderTarget[0].SrcBlend              = D3D12_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp               = D3D12_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].DestBlend             = D3D12_BLEND_ONE;
    blendDesc.RenderTarget[0].SrcBlendAlpha         = D3D12_BLEND_ONE;
    blendDesc.RenderTarget[0].BlendOpAlpha          = D3D12_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].DestBlendAlpha        = D3D12_BLEND_ZERO;

    // RasterizerStateの設定
    D3D12_RASTERIZER_DESC rasterizerDesc{};
    // 裏面を表示しない
    rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
    // 三角形の色を塗りつぶす
    rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

    // DepthStencilStateの設定
    // Depthの機能を有効化する
    depthStencilDesc_.DepthEnable = true;
    // 書き込みする
    depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    // 比較関数はLessEqual。つまり、近ければ描画される
    depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

    // Shaderをコンパイルする
    vertexShaderBlob_ = dxCommon_->GetDxCompiler()->CompileShader(L"resources/Shader/SkyBox.VS.hlsl", L"vs_6_0");
    assert(vertexShaderBlob_ != nullptr);

    pixelShaderBlob_ = dxCommon_->GetDxCompiler()->CompileShader(L"resources/Shader/SkyBox.PS.hlsl", L"ps_6_0");
    assert(pixelShaderBlob_ != nullptr);

    // PSO作成
    D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc = {};
    graphicsPipelineStateDesc.pRootSignature                     = rootSignature_.Get();
    graphicsPipelineStateDesc.InputLayout                        = inputLayoutDesc;
    graphicsPipelineStateDesc.VS                                 = {vertexShaderBlob_->GetBufferPointer(), vertexShaderBlob_->GetBufferSize()};
    graphicsPipelineStateDesc.PS                                 = {pixelShaderBlob_->GetBufferPointer(), pixelShaderBlob_->GetBufferSize()};
    graphicsPipelineStateDesc.BlendState                         = blendDesc;
    graphicsPipelineStateDesc.RasterizerState                    = rasterizerDesc;
    graphicsPipelineStateDesc.NumRenderTargets                   = 1;
    graphicsPipelineStateDesc.RTVFormats[0]                      = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    graphicsPipelineStateDesc.PrimitiveTopologyType              = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    graphicsPipelineStateDesc.SampleDesc.Count                   = 1;
    graphicsPipelineStateDesc.SampleMask                         = D3D12_DEFAULT_SAMPLE_MASK;
    graphicsPipelineStateDesc.DepthStencilState                  = depthStencilDesc_;
    graphicsPipelineStateDesc.DSVFormat                          = DXGI_FORMAT_D24_UNORM_S8_UINT;

    hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&pipelineState_));
    assert(SUCCEEDED(hr));
}


void SkyBoxPipeline::PreDraw(ID3D12GraphicsCommandList* commandList) {
    commandList->SetGraphicsRootSignature(rootSignature_.Get());
    commandList->SetPipelineState(pipelineState_.Get());
}

void SkyBoxPipeline::PreBlendSet(ID3D12GraphicsCommandList* commandList, const BlendMode& blendMode) {
    // SkyBoxは単一のブレンドモードのみ使用
    blendMode;
    commandList;
}

void SkyBoxPipeline::SetEnvironmentalMapTextureHandle(const std::string& texture) {
    environmentalMapTextureHandle_ = TextureManager::GetInstance()->LoadTexture(texture);
}
