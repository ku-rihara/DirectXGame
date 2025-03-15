#include "CopyImageRenderer.h"
#include "DirectXCommon.h"
#include "function/Log.h"
#include <cassert>
#include <wrl/client.h>

CopyImageRenderer* CopyImageRenderer::GetInstance() {
    static CopyImageRenderer instance;
    return &instance;
}

void CopyImageRenderer::Init(DirectXCommon* dxCommon) {
    dxCommon_ = dxCommon;
    CreateGraphicsPipeline();
}

void CopyImageRenderer::CreateGraphicsPipeline() {
    HRESULT hr = 0;

    staticSamplers_[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;  // バイリニアフィルタ
    staticSamplers_[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers_[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers_[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers_[0].MipLODBias = 0.0f;
    staticSamplers_[0].MaxAnisotropy = 1;
    staticSamplers_[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    staticSamplers_[0].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
    staticSamplers_[0].MinLOD = 0.0f;
    staticSamplers_[0].MaxLOD = D3D12_FLOAT32_MAX;
    staticSamplers_[0].ShaderRegister = 0;  // `s0` に対応
    staticSamplers_[0].RegisterSpace = 0;
    staticSamplers_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
  
    // ルートシグネチャ作成
    CreateRootSignature();

    //Shaderをコンパイルする
    vertexShaderBlob_ = dxCommon_->CompileShader(L"resources/Shader/CopyImage.VS.hlsl",
        L"vs_6_0", dxCommon_->GetDxcUtils(), dxCommon_->GetDxcCompiler(), dxCommon_->GetIncludeHandler());
    assert(vertexShaderBlob_ != nullptr);

    pixelShaderBlob_ = dxCommon_->CompileShader(L"resources/Shader/CopyImage.PS.hlsl",
        L"ps_6_0", dxCommon_->GetDxcUtils(), dxCommon_->GetDxcCompiler(), dxCommon_->GetIncludeHandler());
    assert(pixelShaderBlob_ != nullptr);

   
    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
    inputLayoutDesc.pInputElementDescs = nullptr;
    inputLayoutDesc.NumElements = 0;

    // ブレンドステート
    D3D12_BLEND_DESC blendDesc = {};
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    // ラスタライザーステート
    D3D12_RASTERIZER_DESC rasterizerDesc = {};
    rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
    rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

    // グラフィックスパイプラインの設定
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.pRootSignature = rootSignature_.Get();
    psoDesc.VS = { vertexShaderBlob_->GetBufferPointer(), vertexShaderBlob_->GetBufferSize() };
    psoDesc.PS = { pixelShaderBlob_->GetBufferPointer(), pixelShaderBlob_->GetBufferSize() };
    psoDesc.InputLayout = inputLayoutDesc;
    psoDesc.BlendState = blendDesc;
    psoDesc.RasterizerState = rasterizerDesc;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    psoDesc.SampleDesc.Count = 1;
    psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

    hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState_));
    assert(SUCCEEDED(hr));
}

void CopyImageRenderer::CreateRootSignature() {
    HRESULT hr = 0;
    //RootSignatureを作成
    D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
    descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    descriptionRootSignature.pStaticSamplers = staticSamplers_;
    descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers_);
   
   
    // **ディスクリプタレンジ (SRV)**
    D3D12_DESCRIPTOR_RANGE descriptorRange = {};
    descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;  // SRV (テクスチャ) 用
    descriptorRange.NumDescriptors = 1;  // 1つのテクスチャ
    descriptorRange.BaseShaderRegister = 0;  // `t0` に対応
    descriptorRange.RegisterSpace = 0;  // デフォルトのスペース
    descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    // **ルートパラメータ**
    D3D12_ROOT_PARAMETER rootParameters[1] = {};
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[0].DescriptorTable.NumDescriptorRanges = 1;
    rootParameters[0].DescriptorTable.pDescriptorRanges = &descriptorRange;
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;


    // **ルートシグネチャ設定**
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    rootSignatureDesc.NumParameters = _countof(rootParameters);
    rootSignatureDesc.pParameters = rootParameters;
    rootSignatureDesc.NumStaticSamplers = 1;
    rootSignatureDesc.pStaticSamplers = staticSamplers_;
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	
    hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);
    if (FAILED(hr)) {
        Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
        assert(false);
    }

    hr = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
    assert(SUCCEEDED(hr));
}


void CopyImageRenderer::Draw(ID3D12GraphicsCommandList* commandList) {
    commandList->SetPipelineState(pipelineState_.Get());
    commandList->SetGraphicsRootSignature(rootSignature_.Get());
   
    // **プリミティブトポロジーを設定**
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // テクスチャリソースを設定（インデックス0のパラメータに割り当て）
    commandList->SetGraphicsRootDescriptorTable(0, dxCommon_->GetRenderTextureGPUSrvHandle());
    commandList->DrawInstanced(3, 1, 0, 0);
}
