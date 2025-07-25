#include "Outline.h"
#include "3d/ViewProjection.h"
#include "Dx/DirectXCommon.h"
#include "Dx/DxCompiler.h"
#include "Dx/DxRenderTarget.h"
#include"Dx/DxDepthBuffer.h"
#include "function/Log.h"
#include <cassert>
#include <d3dx12.h>
#include<imgui.h>

void Outline::Init(DirectXCommon* dxCommon) {

    vsName_ = L"resources/Shader/OffScreen/Fullscreen.VS.hlsl";
    psName_ = L"resources/Shader/OffScreen/DepthBasedOutline.PS.hlsl";
    BaseOffScreen::Init(dxCommon);
}

void Outline::CreateGraphicsPipeline() {
    HRESULT hr = 0;

    // Smplerの設定
    staticSamplersOutLine_[0].Filter           = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイリニアフィルタ
    staticSamplersOutLine_[0].AddressU         = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0～1の範囲外をリピート
    staticSamplersOutLine_[0].AddressV         = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplersOutLine_[0].AddressW         = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplersOutLine_[0].ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
    staticSamplersOutLine_[0].MaxLOD           = D3D12_FLOAT32_MAX; // ありったけのMipmapを使う
    staticSamplersOutLine_[0].ShaderRegister   = 0; // レジスタ番号0を使う (s0)
    staticSamplersOutLine_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う

    staticSamplersOutLine_[1].Filter           = D3D12_FILTER_MIN_MAG_MIP_POINT; // ポイントフィルタ
    staticSamplersOutLine_[1].AddressU         = D3D12_TEXTURE_ADDRESS_MODE_CLAMP; // クランプ
    staticSamplersOutLine_[1].AddressV         = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    staticSamplersOutLine_[1].AddressW         = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    staticSamplersOutLine_[1].ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
    staticSamplersOutLine_[1].MaxLOD           = D3D12_FLOAT32_MAX;
    staticSamplersOutLine_[1].ShaderRegister   = 1; // レジスタ番号1を使う (s1)
    staticSamplersOutLine_[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う

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

void Outline::CreateRootSignature() {
    HRESULT hr = 0;
    // RootSignatureを作成
    D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
    descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    D3D12_DESCRIPTOR_RANGE descriptorRangeColor[1]            = {};
    descriptorRangeColor[0].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descriptorRangeColor[0].NumDescriptors                    = 1;
    descriptorRangeColor[0].BaseShaderRegister                = 0; // t0
    descriptorRangeColor[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    D3D12_DESCRIPTOR_RANGE descriptorRangeDepth[1]            = {};
    descriptorRangeDepth[0].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descriptorRangeDepth[0].NumDescriptors                    = 1;
    descriptorRangeDepth[0].BaseShaderRegister                = 1; // t1
    descriptorRangeDepth[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    // ルートパラメータ
    D3D12_ROOT_PARAMETER rootParameters[4] = {};

    // color
    rootParameters[0].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[0].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[0].DescriptorTable.pDescriptorRanges   = descriptorRangeColor;
    rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeColor);

    // depth
    rootParameters[1].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[1].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[1].DescriptorTable.pDescriptorRanges   = descriptorRangeDepth;
    rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeDepth);

    // 定数バッファ
    rootParameters[2].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[2].ShaderVisibility          = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[2].Descriptor.ShaderRegister = 0;

    // 定数バッファ
    rootParameters[3].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[3].ShaderVisibility          = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[3].Descriptor.ShaderRegister = 1;

    descriptionRootSignature.pParameters       = rootParameters;
    descriptionRootSignature.NumParameters     = _countof(rootParameters);
    descriptionRootSignature.pStaticSamplers   = staticSamplersOutLine_;
    descriptionRootSignature.NumStaticSamplers = _countof(staticSamplersOutLine_);

    hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);
    if (FAILED(hr)) {
        Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
        assert(false);
    }

    hr = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
    assert(SUCCEEDED(hr));
}

void Outline::SetDrawState(ID3D12GraphicsCommandList* commandList) {
    BaseOffScreen::SetDrawState(commandList);
}

void Outline::CreateConstantBuffer() {
    D3D12_RANGE readRange = {};
    HRESULT hr;

    // param (b0)
    paramDataResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(OutLineParamData));
    hr              = paramDataResource_->Map(0, &readRange, reinterpret_cast<void**>(&paramData_));
    if (FAILED(hr)) {
        // エラー処理
        OutputDebugStringA("ConstBuffer Map failed.\n");
    }

    // material (b1)
    outlineMaterialResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(OutLineMaterial));
    hr                       = outlineMaterialResource_->Map(0, &readRange, reinterpret_cast<void**>(&outlineMaterialData_));
    if (FAILED(hr)) {
        // エラー処理
        OutputDebugStringA("ConstBuffer Map failed.\n");
    }

    outlineMaterialData_->projectionInverse = MakeIdentity4x4();
    paramData_->wightRate                  = 0.2f;
}

void Outline::Draw([[maybe_unused]] ID3D12GraphicsCommandList* commandList) {
    // プリミティブトポロジーを設定
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    outlineMaterialData_->projectionInverse = Inverse(viewProjection_->matProjection_);

    // colorテクスチャ (t0)
    commandList->SetGraphicsRootDescriptorTable(0, dxCommon_->GetDxRenderTarget()->GetRenderTextureGPUSrvHandle());

    // depthテクスチャ (t1)
    commandList->SetGraphicsRootDescriptorTable(1, dxCommon_->GetDepthBuffer()->GetDepthSrvGPUHandle());

    // b0: OutLineParams
    commandList->SetGraphicsRootConstantBufferView(2, paramDataResource_->GetGPUVirtualAddress());

    // b1: OutLineMaterial
    commandList->SetGraphicsRootConstantBufferView(3, outlineMaterialResource_->GetGPUVirtualAddress());

    commandList->DrawInstanced(3, 1, 0, 0);
}

void Outline::DebugParamImGui() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader("Outline")) {
        ImGui::DragFloat("widthRate", &paramData_->wightRate, 0.01f);
    }
#endif
}
