#include "Line3DPipeline.h"
#include "Dx/DirectXCommon.h"
#include"Dx/DxCompiler.h"
#include <cassert>


void Line3DPipeline::Init(DirectXCommon* dxCommon) {
    BasePipeline::Init(dxCommon);
}


void Line3DPipeline::CreateRootSignature() {
  
    D3D12_ROOT_SIGNATURE_DESC desc{};
    desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    D3D12_ROOT_PARAMETER param{};
    param.ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    param.ShaderVisibility          = D3D12_SHADER_VISIBILITY_VERTEX;
    param.Descriptor.ShaderRegister = 0;

    desc.pParameters   = &param;
    desc.NumParameters = 1;

     // シリアライズしてバイナリにする
    SerializeAndCreateRootSignature(desc);
}

void Line3DPipeline::CreateGraphicsPipeline() {

    // InputLayoutの設定を行う
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
    inputElementDescs[0].SemanticName             = "POSITION";
    inputElementDescs[0].SemanticIndex            = 0;
    inputElementDescs[0].Format                   = DXGI_FORMAT_R32G32B32_FLOAT;
    inputElementDescs[0].AlignedByteOffset        = D3D12_APPEND_ALIGNED_ELEMENT;

    inputElementDescs[1].SemanticName      = "COLOR";
    inputElementDescs[1].SemanticIndex     = 0;
    inputElementDescs[1].Format            = DXGI_FORMAT_R32G32B32A32_FLOAT;
    inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
    inputLayoutDesc.pInputElementDescs = inputElementDescs;
    inputLayoutDesc.NumElements        = _countof(inputElementDescs);

    vertexShaderBlob_ = dxCommon_->GetDxCompiler()->CompileShader(L"resources/Shader/Line3D.VS.hlsl", L"vs_6_0");

    pixelShaderBlob_ = dxCommon_->GetDxCompiler()->CompileShader(L"resources/Shader/Line3D.PS.hlsl", L"ps_6_0");

    D3D12_BLEND_DESC blendDesc{};
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    blendDesc.RenderTarget[0].BlendEnable           = TRUE;
    blendDesc.RenderTarget[0].SrcBlend              = D3D12_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp               = D3D12_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].DestBlend             = D3D12_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].SrcBlendAlpha         = D3D12_BLEND_ONE;
    blendDesc.RenderTarget[0].BlendOpAlpha          = D3D12_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].DestBlendAlpha        = D3D12_BLEND_ZERO;

    D3D12_RASTERIZER_DESC rasterizerDesc{};

    rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
    rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

    depthStencilDesc_.DepthEnable    = true;
    depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc_.DepthFunc      = D3D12_COMPARISON_FUNC_LESS_EQUAL;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};
    desc.InputLayout           = inputLayoutDesc;
    desc.pRootSignature        = rootSignature_.Get();
    desc.VS                    = {vertexShaderBlob_->GetBufferPointer(), vertexShaderBlob_->GetBufferSize()};
    desc.PS                    = {pixelShaderBlob_->GetBufferPointer(), pixelShaderBlob_->GetBufferSize()};
    desc.BlendState            = blendDesc;
    desc.RasterizerState       = rasterizerDesc;
    desc.DepthStencilState     = depthStencilDesc_;
    desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    desc.NumRenderTargets      = 1;
    desc.RTVFormats[0]         = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    desc.DSVFormat             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    desc.SampleDesc.Count      = 1;
    desc.SampleMask            = D3D12_DEFAULT_SAMPLE_MASK;

    HRESULT hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&graphicsPipelineState_));
    assert(SUCCEEDED(hr));
    if (FAILED(hr)) {
        return;
    }
}


void Line3DPipeline::PreDraw(ID3D12GraphicsCommandList* commandList) {
    commandList->SetPipelineState(graphicsPipelineState_.Get());
    commandList->SetGraphicsRootSignature(rootSignature_.Get());
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
}

void Line3DPipeline::PreBlendSet(ID3D12GraphicsCommandList* commandList, const BlendMode& blendMode) {
    commandList;
    blendMode;
}