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
    staticSamplers_[0].Filter           = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイリニアフィルタ
    staticSamplers_[0].AddressU         = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0~1の範囲外をリピート
    staticSamplers_[0].AddressV         = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers_[0].AddressW         = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers_[0].ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
    staticSamplers_[0].MaxLOD           = D3D12_FLOAT32_MAX; // ありったけのMipMapを使う
    staticSamplers_[0].ShaderRegister   = 0; // レジスタ番号０
    staticSamplers_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // pxelShaderで使う

    // シャドウマップ用比較サンプラー
    staticSamplers_[1].Filter           = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
    staticSamplers_[1].AddressU         = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    staticSamplers_[1].AddressV         = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    staticSamplers_[1].AddressW         = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    staticSamplers_[1].BorderColor      = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
    staticSamplers_[1].ComparisonFunc   = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    staticSamplers_[1].MaxLOD           = D3D12_FLOAT32_MAX;
    staticSamplers_[1].ShaderRegister   = 1; // レジスタ番号1
    staticSamplers_[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    staticSamplers_[1].MaxAnisotropy    = 1;

    // RootSignatureを作成
    D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
    descriptionRootSignature.Flags             = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    descriptionRootSignature.pStaticSamplers   = staticSamplers_;
    descriptionRootSignature.NumStaticSamplers = 2; // 通常サンプラーとシャドウサンプラーの2個

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
    D3D12_ROOT_PARAMETER rootParameters[14] = {};

    // 0: Material
    rootParameters[0].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使う
    rootParameters[0].ShaderVisibility          = D3D12_SHADER_VISIBILITY_PIXEL; // PxelShaderを使う
    rootParameters[0].Descriptor.ShaderRegister = 0; // レジスタ番号0とバインド

    // 1: TransformationMatrix
    rootParameters[1].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使う
    rootParameters[1].ShaderVisibility          = D3D12_SHADER_VISIBILITY_VERTEX; // VertexShaderを使う
    rootParameters[1].Descriptor.ShaderRegister = 0; // レジスタ番号0とバインド

    // 2: Texture2D
    rootParameters[2].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う
    rootParameters[2].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
    rootParameters[2].DescriptorTable.pDescriptorRanges   = &descriptorRange[0];
    rootParameters[2].DescriptorTable.NumDescriptorRanges = 1;

    // 3: TextureCube
    rootParameters[3].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[3].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[3].DescriptorTable.pDescriptorRanges   = &descriptorRange[1];
    rootParameters[3].DescriptorTable.NumDescriptorRanges = 1;

    // 4: DirectionalLight
    rootParameters[4].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[4].ShaderVisibility          = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[4].Descriptor.ShaderRegister = 1;

    // 5: Camera
    rootParameters[5].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[5].ShaderVisibility          = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[5].Descriptor.ShaderRegister = 2;

    // 6: PointLights
    rootParameters[6].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[6].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[6].DescriptorTable.pDescriptorRanges   = &descriptorRange[2];
    rootParameters[6].DescriptorTable.NumDescriptorRanges = 1;

    // 7: SpotLights
    rootParameters[7].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[7].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[7].DescriptorTable.pDescriptorRanges   = &descriptorRange[3];
    rootParameters[7].DescriptorTable.NumDescriptorRanges = 1;

    // 8: AreaLight
    rootParameters[8].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[8].ShaderVisibility          = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[8].Descriptor.ShaderRegister = 3;

    // 9: AmbientLight
    rootParameters[9].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[9].ShaderVisibility          = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[9].Descriptor.ShaderRegister = 4;

    // 10: LightCountData
    rootParameters[10].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[10].ShaderVisibility          = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[10].Descriptor.ShaderRegister = 5;

    // 11: ShadowMap
    rootParameters[11].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[11].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[11].DescriptorTable.pDescriptorRanges   = &descriptorRange[4];
    rootParameters[11].DescriptorTable.NumDescriptorRanges = 1;

    // 12: lightTransform
    rootParameters[12].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[12].ShaderVisibility          = D3D12_SHADER_VISIBILITY_VERTEX;
    rootParameters[12].Descriptor.ShaderRegister = 1;

    // 13: dissolve
    rootParameters[13].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[13].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[13].DescriptorTable.pDescriptorRanges   = &descriptorRange[5];
    rootParameters[13].DescriptorTable.NumDescriptorRanges = 1;

    descriptionRootSignature.pParameters   = rootParameters; // ルートパラメーターの配列
    descriptionRootSignature.NumParameters = _countof(rootParameters); // 配列の長さ

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
