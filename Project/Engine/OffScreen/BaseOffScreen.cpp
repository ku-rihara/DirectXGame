#include "BaseOffScreen.h"
#include "Dx/DirectXCommon.h"
#include"Dx/DxCompiler.h"
#include "function/Log.h"
#include <cassert>
#include <d3dx12.h>
#include <wrl/client.h>

void BaseOffScreen::Init(DirectXCommon* dxCommon) {
    dxCommon_ = dxCommon;
    CreateGraphicsPipeline();
}

void BaseOffScreen::CreateGraphicsPipeline() {
    HRESULT hr = 0;

    CreateRootSignature();

    // vs
    vertexShaderBlob_ = dxCommon_->GetDxCompiler()->CompileShader(vsName_, L"vs_6_0");
    // ps
    pixelShaderBlob_ = dxCommon_->GetDxCompiler()->CompileShader(psName_, L"ps_6_0");

    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};
    desc.pRootSignature        = rootSignature_.Get();
    desc.VS                    = {vertexShaderBlob_->GetBufferPointer(), vertexShaderBlob_->GetBufferSize()};
    desc.PS                    = {pixelShaderBlob_->GetBufferPointer(), pixelShaderBlob_->GetBufferSize()};
    desc.BlendState            = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    desc.RasterizerState       = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    desc.InputLayout           = {nullptr, 0};
    desc.NumRenderTargets      = 1;
    desc.RTVFormats[0]         = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    desc.SampleDesc.Count      = 1;
    desc.SampleMask            = D3D12_DEFAULT_SAMPLE_MASK;

    hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipelineStates_));
    assert(SUCCEEDED(hr));
}

void BaseOffScreen::CreateRootSignature() {
    HRESULT hr = 0;
    // RootSignatureを作成
    D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
    descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    // ディスクリプタレンジ
    D3D12_DESCRIPTOR_RANGE descriptorRange[1]            = {};
    descriptorRange[0].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRV (テクスチャ) 用
    descriptorRange[0].NumDescriptors                    = 1; // 1つのテクスチャ
    descriptorRange[0].BaseShaderRegister                = 0; // `t0` に対応
    descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    // ルートパラメータ
    D3D12_ROOT_PARAMETER rootParameters[1]                = {};
    rootParameters[0].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[0].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[0].DescriptorTable.pDescriptorRanges   = descriptorRange;
    rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);
    descriptionRootSignature.pParameters                  = rootParameters; // ルートパラメータ配列へのポインタ
    descriptionRootSignature.NumParameters                = _countof(rootParameters); // 配列の長さ

    // Smplerの設定
    staticSamplers_[0].Filter                  = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイリニアフィルタ
    staticSamplers_[0].AddressU                = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0～1の範囲外をリピート
    staticSamplers_[0].AddressV                = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers_[0].AddressW                = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers_[0].ComparisonFunc          = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
    staticSamplers_[0].MaxLOD                  = D3D12_FLOAT32_MAX; // ありったけのMipmapを使う
    staticSamplers_[0].ShaderRegister          = 0; // レジスタ番号0を使う
    staticSamplers_[0].ShaderVisibility        = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
    descriptionRootSignature.pStaticSamplers   = staticSamplers_;
    descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers_);

    hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);
    if (FAILED(hr)) {
        Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
        assert(false);
    }

    hr = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
    assert(SUCCEEDED(hr));
}

void BaseOffScreen::SetDrawState(ID3D12GraphicsCommandList* commandList) {
    commandList->SetPipelineState(pipelineStates_.Get());
    commandList->SetGraphicsRootSignature(rootSignature_.Get());
 }

