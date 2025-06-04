#include "FullscreenRenderer.h"
#include"Dx/DirectXCommon.h"
#include "function/Log.h"
#include <cassert>
#include <d3dx12.h>
#include <imgui.h>
#include <wrl/client.h>

FullscreenRenderer* FullscreenRenderer::GetInstance() {
    static FullscreenRenderer instance;
    return &instance;
}

void FullscreenRenderer::Init(DirectXCommon* dxCommon) {
    dxCommon_ = dxCommon;

    psFiles_ = {
        L"resources/Shader/OffScreen/Fullscreen.PS.hlsl",
        L"resources/Shader/OffScreen/Grayscale.PS.hlsl",
        L"resources/Shader/OffScreen/Vignette.PS.hlsl",
        L"resources/Shader/OffScreen/GaussianFilter.PS.hlsl"
        /*L"resources/Shader/OffScreen/RadialBlur.PS.hlsl"*/};

    CreateGraphicsPipeline();
}

void FullscreenRenderer::CreateGraphicsPipeline() {
    HRESULT hr = 0;

    CreateRootSignature();

    vertexShaderBlob_ = dxCommon_->CompileShader(
        L"resources/Shader/OffScreen/Fullscreen.VS.hlsl",
        L"vs_6_0", dxCommon_->GetDxcUtils(), dxCommon_->GetDxcCompiler(), dxCommon_->GetIncludeHandler());

    for (size_t i = 0; i < psFiles_.size(); ++i) {
        pixelShaderBlob_[i] = dxCommon_->CompileShader(
            psFiles_[i].c_str(), L"ps_6_0",
            dxCommon_->GetDxcUtils(), dxCommon_->GetDxcCompiler(), dxCommon_->GetIncludeHandler());

        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};
        desc.pRootSignature        = rootSignature_.Get();
        desc.VS                    = {vertexShaderBlob_->GetBufferPointer(), vertexShaderBlob_->GetBufferSize()};
        desc.PS                    = {pixelShaderBlob_[i]->GetBufferPointer(), pixelShaderBlob_[i]->GetBufferSize()};
        desc.BlendState            = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        desc.RasterizerState       = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        desc.InputLayout           = {nullptr, 0};
        desc.NumRenderTargets      = 1;
        desc.RTVFormats[0]         = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        desc.SampleDesc.Count      = 1;
        desc.SampleMask            = D3D12_DEFAULT_SAMPLE_MASK;

        hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipelineStates_[i]));
        assert(SUCCEEDED(hr));
    }
}

void FullscreenRenderer::CreateRootSignature() {
    HRESULT hr = 0;
    // RootSignatureを作成
    D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
    descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    // **ディスクリプタレンジ (SRV)**
    D3D12_DESCRIPTOR_RANGE descriptorRange[1]            = {};
    descriptorRange[0].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRV (テクスチャ) 用
    descriptorRange[0].NumDescriptors                    = 1; // 1つのテクスチャ
    descriptorRange[0].BaseShaderRegister                = 0; // `t0` に対応
    descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    // **ルートパラメータ**
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

void FullscreenRenderer::Draw(ID3D12GraphicsCommandList* commandList) {
    commandList->SetPipelineState(pipelineStates_[static_cast<size_t>(currentMode_)].Get());
    commandList->SetGraphicsRootSignature(rootSignature_.Get());

    // **プリミティブトポロジーを設定**
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // テクスチャリソースを設定（インデックス0のパラメータに割り当て）
    commandList->SetGraphicsRootDescriptorTable(0, dxCommon_->GetRenderTextureGPUSrvHandle());
    commandList->DrawInstanced(3, 1, 0, 0);
}

void FullscreenRenderer::DrawImGui() {
#ifdef DEBUG

    if (ImGui::Begin("CopyImageRenderer")) {
        const char* modeNames[] = {"None", "Gray", "Vignette", "Gaus"/*, "RadialBlur"*/};
        int mode                = static_cast<int>(currentMode_);
        if (ImGui::Combo("OffScreenMode", &mode, modeNames, IM_ARRAYSIZE(modeNames))) {
            currentMode_ = static_cast<OffScreenMode>(mode);
        }
    }
    ImGui::End();
#endif // DEBUG
}
