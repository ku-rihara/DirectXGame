#include "SkinningObject3DPipeline.h"
// Function
#include "function/Log.h"
#include <cassert>
#include <string>


SkinningObject3DPipeline* SkinningObject3DPipeline::GetInstance() {
    static SkinningObject3DPipeline instance;
    return &instance;
}

void SkinningObject3DPipeline::Init(DirectXCommon* dxCommon) {

    // 引数で受けとる
    dxCommon_ = dxCommon;
    // グラフィックスパイプラインの生成
    CreateGraphicsPipeline();
}

void SkinningObject3DPipeline::CreateGraphicsPipeline() {

    HRESULT hr = 0;

    staticSamplers_[0].Filter           = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイリニアフィルタ
    staticSamplers_[0].AddressU         = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0~1の範囲外をリピート
    staticSamplers_[0].AddressV         = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers_[0].AddressW         = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers_[0].ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
    staticSamplers_[0].MaxLOD           = D3D12_FLOAT32_MAX; // ありったけのMipMapを使う
    staticSamplers_[0].ShaderRegister   = 0; // レジスタ番号０
    staticSamplers_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // pxelShaderで使う

    CreateRootSignature();

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

    // BlendMode::None のブレンド設定を透明度対応に変更
    D3D12_BLEND_DESC blendDescNone                      = {};
    blendDescNone.RenderTarget[0].BlendEnable           = TRUE; // ブレンドを有効化
    blendDescNone.RenderTarget[0].SrcBlend              = D3D12_BLEND_SRC_ALPHA;
    blendDescNone.RenderTarget[0].DestBlend             = D3D12_BLEND_INV_SRC_ALPHA; // 1 - SrcAlpha
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
    vertexShaderBlob_ = dxCommon_->CompileShader(L"resources/Shader/Object3d.VS.hlsl",
        L"vs_6_0", dxCommon_->GetDxcUtils(), dxCommon_->GetDxcCompiler(), dxCommon_->GetIncludeHandler());
    assert(vertexShaderBlob_ != nullptr);

    pixelShaderBlob_ = dxCommon_->CompileShader(L"resources/Shader/Object3d.PS.hlsl",
        L"ps_6_0", dxCommon_->GetDxcUtils(), dxCommon_->GetDxcCompiler(), dxCommon_->GetIncludeHandler());
    assert(pixelShaderBlob_ != nullptr);

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

void SkinningObject3DPipeline::CreateRootSignature() {
    HRESULT hr = 0;
    // RootSignatureを作成
    D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
    descriptionRootSignature.Flags             = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    descriptionRootSignature.pStaticSamplers   = staticSamplers_;
    descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers_);

    D3D12_DESCRIPTOR_RANGE descriptorRange[2]            = {};
    descriptorRange[0].BaseShaderRegister                = 0; // 0から始まる
    descriptorRange[0].NumDescriptors                    = 1; // 数は1つ
    descriptorRange[0].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
    descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算

    descriptorRange[1].BaseShaderRegister                = 1; // 1から始まる
    descriptorRange[1].NumDescriptors                    = 1; // 数は1つ
    descriptorRange[1].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
    descriptorRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算


    // RootParameterを作成
    D3D12_ROOT_PARAMETER rootParameters[10]      = {};
    rootParameters[0].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使う
    rootParameters[0].ShaderVisibility          = D3D12_SHADER_VISIBILITY_PIXEL; // PxelShaderを使う
    rootParameters[0].Descriptor.ShaderRegister = 0; // レジスタ番号0とバインド

    rootParameters[1].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使う
    rootParameters[1].ShaderVisibility          = D3D12_SHADER_VISIBILITY_VERTEX; // VertexShaderを使う
    rootParameters[1].Descriptor.ShaderRegister = 0; // レジスタ番号0とバインド

      // TextureCube（PixelShader）→ t0（DescriptorTable）
    rootParameters[2].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う
    rootParameters[2].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
    rootParameters[2].DescriptorTable.pDescriptorRanges   = &descriptorRange[0];
    rootParameters[2].DescriptorTable.NumDescriptorRanges = 1;

     // TextureCube（PixelShader）→ t1（DescriptorTable）
    rootParameters[3].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[3].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[3].DescriptorTable.pDescriptorRanges   = &descriptorRange[1];
    rootParameters[3].DescriptorTable.NumDescriptorRanges = 1;

    // Lambart
    rootParameters[4].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[4].ShaderVisibility          = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[4].Descriptor.ShaderRegister = 1;
    // Half Lambart
    rootParameters[5].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[5].ShaderVisibility          = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[5].Descriptor.ShaderRegister = 2;
    // PointLight
    rootParameters[6].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[6].ShaderVisibility          = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[6].Descriptor.ShaderRegister = 3;
    // SpotLight
    rootParameters[7].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[7].ShaderVisibility          = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[7].Descriptor.ShaderRegister = 4;
    // AreaLight
    rootParameters[8].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[8].ShaderVisibility          = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[8].Descriptor.ShaderRegister = 5;

    // AreaLight8
    rootParameters[9].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[9].ShaderVisibility          = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[9].Descriptor.ShaderRegister = 6;

    descriptionRootSignature.pParameters   = rootParameters; // ルートパラメーターの配列
    descriptionRootSignature.NumParameters = _countof(rootParameters); // 配列の長さ

    // Object*************************************************************************************************************************
    // シリアライズしてバイナリにする
    signatureBlob_ = nullptr;
    errorBlob_     = nullptr;
    hr             = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);
    if (FAILED(hr)) {
        Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
        assert(false);
    }
    // バイナリを元に生成
    rootSignature_ = nullptr;
    hr             = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
    assert(SUCCEEDED(hr));
}

void SkinningObject3DPipeline::PreDraw(ID3D12GraphicsCommandList* commandList) {
    // RootSignatureを設定
    commandList->SetGraphicsRootSignature(rootSignature_.Get());
}

void SkinningObject3DPipeline::PreBlendSet(ID3D12GraphicsCommandList* commandList, BlendMode blendMode) {
    switch (blendMode) {
    case BlendMode::None:
        commandList->SetPipelineState(graphicsPipelineStateNone_.Get());
        break;
    case BlendMode::Add:
        commandList->SetPipelineState(graphicsPipelineStateAdd_.Get());
        break;
    
    }
}
