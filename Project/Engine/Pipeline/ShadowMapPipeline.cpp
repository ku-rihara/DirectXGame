#include "ShadowMapPipeline.h"
#include "Dx/DxCompiler.h"
// Function
#include "function/Log.h"
#include <cassert>
#include <string>

ShadowMapPipeline* ShadowMapPipeline::GetInstance() {
    static ShadowMapPipeline instance;
    return &instance;
}

void ShadowMapPipeline::Init(DirectXCommon* dxCommon) {
    // 引数で受けとる
    dxCommon_ = dxCommon;
    // グラフィックスパイプラインの生成
    CreateGraphicsPipeline();
}

void ShadowMapPipeline::CreateGraphicsPipeline() {
    HRESULT hr = 0;

    //// Sampler 設定（この段階では未使用なので削除も可）
    //staticSamplers_[0].Filter           = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    //staticSamplers_[0].AddressU         = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    //staticSamplers_[0].AddressV         = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    //staticSamplers_[0].AddressW         = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    //staticSamplers_[0].ComparisonFunc   = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    //staticSamplers_[0].MaxLOD           = D3D12_FLOAT32_MAX;
    //staticSamplers_[0].ShaderRegister   = 0;
    //staticSamplers_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

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

    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
    inputLayoutDesc.pInputElementDescs      = inputElementDescs;
    inputLayoutDesc.NumElements             = _countof(inputElementDescs);

    depthStencilDesc_.DepthEnable    = true;
    depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc_.DepthFunc      = D3D12_COMPARISON_FUNC_LESS;
    depthStencilDesc_.StencilEnable  = false;

    vertexShaderBlob_ = dxCommon_->GetDxCompiler()->CompileShader(L"resources/Shader/ShadowMap/ShadowMap.VS.hlsl", L"vs_6_0");
    assert(vertexShaderBlob_ != nullptr);

    pixelShaderBlob_ = nullptr;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = {};
    pipelineDesc.pRootSignature                     = rootSignature_.Get();
    pipelineDesc.InputLayout                        = inputLayoutDesc;
    pipelineDesc.VS                                 = {vertexShaderBlob_->GetBufferPointer(), vertexShaderBlob_->GetBufferSize()};
    pipelineDesc.PS                                 = {}; // 空にする
    pipelineDesc.RasterizerState.FillMode           = D3D12_FILL_MODE_SOLID;
    pipelineDesc.RasterizerState.CullMode           = D3D12_CULL_MODE_BACK;
    pipelineDesc.RasterizerState.DepthClipEnable    = true;

    pipelineDesc.DepthStencilState     = depthStencilDesc_;
    pipelineDesc.BlendState            = {}; // 不要なBlendStateを初期化
    pipelineDesc.NumRenderTargets      = 0; // カラーバッファ出力なし
    pipelineDesc.RTVFormats[0]         = DXGI_FORMAT_UNKNOWN;
    pipelineDesc.DSVFormat             = DXGI_FORMAT_D32_FLOAT; // 深度のみ
    pipelineDesc.SampleMask            = D3D12_DEFAULT_SAMPLE_MASK;
    pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pipelineDesc.SampleDesc.Count      = 1;

    hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState_));
    assert(SUCCEEDED(hr));
}

void ShadowMapPipeline::CreateRootSignature() {
    HRESULT hr = 0;

    D3D12_ROOT_PARAMETER rootParams[2] = {};

    // b0: WorldMatrix
    rootParams[0].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParams[0].Descriptor.ShaderRegister = 0;
    rootParams[0].ShaderVisibility          = D3D12_SHADER_VISIBILITY_VERTEX;

    // b1: LightMatrix
    rootParams[1].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParams[1].Descriptor.ShaderRegister = 1;
    rootParams[1].ShaderVisibility          = D3D12_SHADER_VISIBILITY_VERTEX;

    D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature = {};
    descriptionRootSignature.pParameters               = rootParams; // ルートパラメーターの配列
    descriptionRootSignature.NumParameters             = _countof(rootParams); // 配列の長さ
    descriptionRootSignature.Flags                     = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    descriptionRootSignature.NumStaticSamplers = 0;
    descriptionRootSignature.pStaticSamplers   = nullptr;

    hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);
    if (FAILED(hr)) {
        Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
        assert(false);
    }

    hr = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
    assert(SUCCEEDED(hr));
}

void ShadowMapPipeline::PreDraw(ID3D12GraphicsCommandList* commandList) {
    // RootSignatureを設定
    commandList->SetGraphicsRootSignature(rootSignature_.Get());
    commandList->SetPipelineState(pipelineState_.Get());
}