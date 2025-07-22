#include "Outline.h"
#include "Dx/DirectXCommon.h"
#include"base/WinApp.h"
#include "function/Log.h"
#include <cassert>


void Outline::Init(DirectXCommon* dxCommon) {

    vsName_ = L"resources/Shader/OffScreen/Fullscreen.VS.hlsl";
    psName_ = L"resources/Shader/OffScreen/LuminanceBasedOutline.PS.hlsl";
    BaseOffScreen::Init(dxCommon);
}

void Outline::CreateGraphicsPipeline() {
    BaseOffScreen::CreateGraphicsPipeline();
}

void Outline::CreateRootSignature() {
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
    D3D12_ROOT_PARAMETER rootParameters[2]                = {};
    rootParameters[0].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[0].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[0].DescriptorTable.pDescriptorRanges   = descriptorRange;
    rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

    rootParameters[1].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[1].ShaderVisibility          = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[1].Descriptor.ShaderRegister = 0;
    descriptionRootSignature.pParameters        = rootParameters; // ルートパラメータ配列へのポインタ
    descriptionRootSignature.NumParameters      = _countof(rootParameters); // 配列の長さ

    descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers_);

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

    // 定数バッファを生成
    uvStepResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(UVStepData));

    // 定数バッファのマッピング
    D3D12_RANGE readRange = {};
    HRESULT hr            = uvStepResource_->Map(0, &readRange, reinterpret_cast<void**>(&uvStepData_));
    if (FAILED(hr)) {
        // エラー処理
        OutputDebugStringA("ConstBuffer Map failed.\n");
    }

    uvStepData_->size = {WinApp::kWindowWidth, WinApp::kWindowHeight};
}

void Outline::SetCommand([[maybe_unused]] ID3D12GraphicsCommandList* commandList) {
}


void Outline::DebugParamImGui() {
}