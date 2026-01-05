#include "Object3DPipeline.h"

using namespace KetaEngine;
#include "Dx/DxCompiler.h"
// Function
#include "function/Log.h"
#include <cassert>
#include <string>


void Object3DPipeline::Init(DirectXCommon* dxCommon) {
    BasePipeline::Init(dxCommon);
}

void Object3DPipeline::CreateRootSignature() {

    // 通常のサンプラー
    staticSamplers_[0].Filter           = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    staticSamplers_[0].AddressU         = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers_[0].AddressV         = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers_[0].AddressW         = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers_[0].ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER;
    staticSamplers_[0].MaxLOD           = D3D12_FLOAT32_MAX;
    staticSamplers_[0].ShaderRegister   = 0;
    staticSamplers_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    // シャドウマップ用比較サンプラー
    staticSamplers_[1].Filter           = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
    staticSamplers_[1].AddressU         = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    staticSamplers_[1].AddressV         = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    staticSamplers_[1].AddressW         = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    staticSamplers_[1].BorderColor      = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
    staticSamplers_[1].ComparisonFunc   = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    staticSamplers_[1].MaxLOD           = D3D12_FLOAT32_MAX;
    staticSamplers_[1].ShaderRegister   = 1;
    staticSamplers_[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    staticSamplers_[1].MaxAnisotropy    = 1;

    // RootSignatureを作成
    D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
    descriptionRootSignature.Flags             = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    descriptionRootSignature.pStaticSamplers   = staticSamplers_;
    descriptionRootSignature.NumStaticSamplers = 2;

    // DescriptorRangeの設定
    D3D12_DESCRIPTOR_RANGE descriptorRange[6] = {};

    // テクスチャ (t0)
    descriptorRange[0].BaseShaderRegister                = 0;
    descriptorRange[0].NumDescriptors                    = 1;
    descriptorRange[0].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    // 環境テクスチャ (t1)
    descriptorRange[1].BaseShaderRegister                = 1;
    descriptorRange[1].NumDescriptors                    = 1;
    descriptorRange[1].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descriptorRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    // ポイントライト (t2)
    descriptorRange[2].BaseShaderRegister                = 2;
    descriptorRange[2].NumDescriptors                    = 1;
    descriptorRange[2].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descriptorRange[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    // スポットライト (t3)
    descriptorRange[3].BaseShaderRegister                = 3;
    descriptorRange[3].NumDescriptors                    = 1;
    descriptorRange[3].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descriptorRange[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    // シャドウマップ (t4)
    descriptorRange[4].BaseShaderRegister                = 4;
    descriptorRange[4].NumDescriptors                    = 1;
    descriptorRange[4].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descriptorRange[4].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    // dissolve (t5)
    descriptorRange[5].BaseShaderRegister                = 5;
    descriptorRange[5].NumDescriptors                    = 1;
    descriptorRange[5].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descriptorRange[5].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    // RootParameterを作成
    D3D12_ROOT_PARAMETER rootParameters[static_cast<UINT>(Object3DRootParameter::Count)] = {};

    // Material (b0, Pixel)
    rootParameters[static_cast<UINT>(Object3DRootParameter::Material)].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[static_cast<UINT>(Object3DRootParameter::Material)].ShaderVisibility          = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[static_cast<UINT>(Object3DRootParameter::Material)].Descriptor.ShaderRegister = 0;

    // TransformationMatrix (b0, Vertex)
    rootParameters[static_cast<UINT>(Object3DRootParameter::TransformationMatrix)].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[static_cast<UINT>(Object3DRootParameter::TransformationMatrix)].ShaderVisibility          = D3D12_SHADER_VISIBILITY_VERTEX;
    rootParameters[static_cast<UINT>(Object3DRootParameter::TransformationMatrix)].Descriptor.ShaderRegister = 0;

    // Texture2D (t0, Pixel)
    rootParameters[static_cast<UINT>(Object3DRootParameter::Texture2D)].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[static_cast<UINT>(Object3DRootParameter::Texture2D)].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[static_cast<UINT>(Object3DRootParameter::Texture2D)].DescriptorTable.pDescriptorRanges   = &descriptorRange[0];
    rootParameters[static_cast<UINT>(Object3DRootParameter::Texture2D)].DescriptorTable.NumDescriptorRanges = 1;

    // TextureCube (t1, Pixel)
    rootParameters[static_cast<UINT>(Object3DRootParameter::TextureCube)].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[static_cast<UINT>(Object3DRootParameter::TextureCube)].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[static_cast<UINT>(Object3DRootParameter::TextureCube)].DescriptorTable.pDescriptorRanges   = &descriptorRange[1];
    rootParameters[static_cast<UINT>(Object3DRootParameter::TextureCube)].DescriptorTable.NumDescriptorRanges = 1;

    // DirectionalLight (b1, Pixel)
    rootParameters[static_cast<UINT>(Object3DRootParameter::DirectionalLight)].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[static_cast<UINT>(Object3DRootParameter::DirectionalLight)].ShaderVisibility          = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[static_cast<UINT>(Object3DRootParameter::DirectionalLight)].Descriptor.ShaderRegister = 1;

    // Camera (b2, Pixel)
    rootParameters[static_cast<UINT>(Object3DRootParameter::Camera)].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[static_cast<UINT>(Object3DRootParameter::Camera)].ShaderVisibility          = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[static_cast<UINT>(Object3DRootParameter::Camera)].Descriptor.ShaderRegister = 2;

    // PointLights (t2, Pixel)
    rootParameters[static_cast<UINT>(Object3DRootParameter::PointLights)].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[static_cast<UINT>(Object3DRootParameter::PointLights)].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[static_cast<UINT>(Object3DRootParameter::PointLights)].DescriptorTable.pDescriptorRanges   = &descriptorRange[2];
    rootParameters[static_cast<UINT>(Object3DRootParameter::PointLights)].DescriptorTable.NumDescriptorRanges = 1;

    // SpotLights (t3, Pixel)
    rootParameters[static_cast<UINT>(Object3DRootParameter::SpotLights)].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[static_cast<UINT>(Object3DRootParameter::SpotLights)].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[static_cast<UINT>(Object3DRootParameter::SpotLights)].DescriptorTable.pDescriptorRanges   = &descriptorRange[3];
    rootParameters[static_cast<UINT>(Object3DRootParameter::SpotLights)].DescriptorTable.NumDescriptorRanges = 1;

    // AreaLight (b3, Pixel)
    rootParameters[static_cast<UINT>(Object3DRootParameter::AreaLight)].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[static_cast<UINT>(Object3DRootParameter::AreaLight)].ShaderVisibility          = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[static_cast<UINT>(Object3DRootParameter::AreaLight)].Descriptor.ShaderRegister = 3;

    // AmbientLight (b4, Pixel)
    rootParameters[static_cast<UINT>(Object3DRootParameter::AmbientLight)].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[static_cast<UINT>(Object3DRootParameter::AmbientLight)].ShaderVisibility          = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[static_cast<UINT>(Object3DRootParameter::AmbientLight)].Descriptor.ShaderRegister = 4;

    // LightCountData (b5, Pixel)
    rootParameters[static_cast<UINT>(Object3DRootParameter::LightCountData)].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[static_cast<UINT>(Object3DRootParameter::LightCountData)].ShaderVisibility          = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[static_cast<UINT>(Object3DRootParameter::LightCountData)].Descriptor.ShaderRegister = 5;

    // ShadowMap (t4, Pixel)
    rootParameters[static_cast<UINT>(Object3DRootParameter::ShadowMap)].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[static_cast<UINT>(Object3DRootParameter::ShadowMap)].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[static_cast<UINT>(Object3DRootParameter::ShadowMap)].DescriptorTable.pDescriptorRanges   = &descriptorRange[4];
    rootParameters[static_cast<UINT>(Object3DRootParameter::ShadowMap)].DescriptorTable.NumDescriptorRanges = 1;

    // LightTransform (b1, Vertex)
    rootParameters[static_cast<UINT>(Object3DRootParameter::LightTransform)].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[static_cast<UINT>(Object3DRootParameter::LightTransform)].ShaderVisibility          = D3D12_SHADER_VISIBILITY_VERTEX;
    rootParameters[static_cast<UINT>(Object3DRootParameter::LightTransform)].Descriptor.ShaderRegister = 1;

    // Dissolve (t5, Pixel)
    rootParameters[static_cast<UINT>(Object3DRootParameter::Dissolve)].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[static_cast<UINT>(Object3DRootParameter::Dissolve)].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[static_cast<UINT>(Object3DRootParameter::Dissolve)].DescriptorTable.pDescriptorRanges   = &descriptorRange[5];
    rootParameters[static_cast<UINT>(Object3DRootParameter::Dissolve)].DescriptorTable.NumDescriptorRanges = 1;

    descriptionRootSignature.pParameters   = rootParameters;
    descriptionRootSignature.NumParameters = static_cast<UINT>(Object3DRootParameter::Count);

    // シリアライズしてバイナリにする
    SerializeAndCreateRootSignature(descriptionRootSignature);
}


void Object3DPipeline::CreateGraphicsPipeline() {

    HRESULT hr = 0;

    // InputLayoutの設定を行う
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
    inputElementDescs[0].SemanticName             = "POSITION";
    inputElementDescs[0].SemanticIndex            = 0;
    inputElementDescs[0].Format                   = DXGI_FORMAT_R32G32B32A32_FLOAT;
    inputElementDescs[0].AlignedByteOffset        = D3D12_APPEND_ALIGNED_ELEMENT;

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

    // ADD ブレンド設定
    D3D12_BLEND_DESC blendDescAdd                      = {};
    blendDescAdd.RenderTarget[0].BlendEnable           = TRUE;
    blendDescAdd.RenderTarget[0].SrcBlend              = D3D12_BLEND_SRC_ALPHA;
    blendDescAdd.RenderTarget[0].DestBlend             = D3D12_BLEND_ONE;
    blendDescAdd.RenderTarget[0].BlendOp               = D3D12_BLEND_OP_ADD;
    blendDescAdd.RenderTarget[0].SrcBlendAlpha         = D3D12_BLEND_ONE;
    blendDescAdd.RenderTarget[0].DestBlendAlpha        = D3D12_BLEND_ZERO;
    blendDescAdd.RenderTarget[0].BlendOpAlpha          = D3D12_BLEND_OP_ADD;
    blendDescAdd.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    // BlendMode None
    D3D12_BLEND_DESC blendDescNone                      = {};
    blendDescNone.RenderTarget[0].BlendEnable           = TRUE;
    blendDescNone.RenderTarget[0].SrcBlend              = D3D12_BLEND_SRC_ALPHA;
    blendDescNone.RenderTarget[0].DestBlend             = D3D12_BLEND_INV_SRC_ALPHA;
    blendDescNone.RenderTarget[0].BlendOp               = D3D12_BLEND_OP_ADD;
    blendDescNone.RenderTarget[0].SrcBlendAlpha         = D3D12_BLEND_ONE;
    blendDescNone.RenderTarget[0].DestBlendAlpha        = D3D12_BLEND_ZERO;
    blendDescNone.RenderTarget[0].BlendOpAlpha          = D3D12_BLEND_OP_ADD;
    blendDescNone.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    // RasterizerStateの設定
    D3D12_RASTERIZER_DESC rasterizerDesc{};
    // 裏面を表示しない
    rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
    // 三角形の色を塗りつぶす
    rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

    // DepthStencilStateの設定-------------------------------------
    // Depthの機能を有効化する
    depthStencilDesc_.DepthEnable = true;
    // 書き込みする
    depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    // 比較関数はLessEqual。つまり、近ければ描画される
    depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

    // Shaderをコンパイルする
    vertexShaderBlob_ = dxCommon_->GetDxCompiler()->CompileShader(L"resources/Shader/Object3d.VS.hlsl", L"vs_6_0");
    pixelShaderBlob_  = dxCommon_->GetDxCompiler()->CompileShader(L"resources/Shader/Object3d.PS.hlsl", L"ps_6_0");

    // PSO作成用関数
    auto CreatePSO = [&](D3D12_BLEND_DESC& blendDesc, Microsoft::WRL::ComPtr<ID3D12PipelineState>& pso) {
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

        hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&pso));
        assert(SUCCEEDED(hr));
    };

    // 各ブレンドモードのPSOを作成
    CreatePSO(blendDescAdd, graphicsPipelineStateAdd_);
    CreatePSO(blendDescNone, graphicsPipelineStateNone_);
}


void Object3DPipeline::PreDraw(ID3D12GraphicsCommandList* commandList) {
    // RootSignatureを設定
    commandList->SetGraphicsRootSignature(rootSignature_.Get());
}

void Object3DPipeline::PreBlendSet(ID3D12GraphicsCommandList* commandList,const BlendMode& blendMode) {
    switch (blendMode) {
    case BlendMode::None:
        commandList->SetPipelineState(graphicsPipelineStateNone_.Get());
        break;
    case BlendMode::Add:
        commandList->SetPipelineState(graphicsPipelineStateAdd_.Get());
        break;
    }
}
