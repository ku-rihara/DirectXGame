#include "Distortion.h"

using namespace KetaEngine;
#include "Base/Dx/DirectXCommon.h"
#include "Base/Dx/DxRenderTarget.h"
#include "function/Log.h"
#include <cassert>
#include <d3dx12.h>

///============================================================
/// 初期化
///============================================================
void Distortion::Init(DirectXCommon* dxCommon) {
    vsName_ = L"resources/Shader/PostEffect/Fullscreen.VS.hlsl";
    psName_ = L"resources/Shader/PostEffect/Distortion.PS.hlsl";
    BasePostEffect::Init(dxCommon);
}

///============================================================
/// ルートシグネチャ作成
/// t0: シーンテクスチャ
/// t1: 歪みテクスチャ（R16G16F）
///============================================================
void Distortion::CreateRootSignature() {
    HRESULT hr = 0;

    D3D12_ROOT_SIGNATURE_DESC desc{};
    desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    // t0: シーンテクスチャ
    D3D12_DESCRIPTOR_RANGE sceneRange{};
    sceneRange.RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    sceneRange.NumDescriptors                    = 1;
    sceneRange.BaseShaderRegister                = 0; // t0
    sceneRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    // t1: 歪みテクスチャ
    D3D12_DESCRIPTOR_RANGE distortionRange{};
    distortionRange.RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    distortionRange.NumDescriptors                    = 1;
    distortionRange.BaseShaderRegister                = 1; // t1
    distortionRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    D3D12_ROOT_PARAMETER rootParameters[2] = {};

    // [0] シーンテクスチャ (t0)
    rootParameters[0].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[0].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[0].DescriptorTable.pDescriptorRanges   = &sceneRange;
    rootParameters[0].DescriptorTable.NumDescriptorRanges = 1;

    // [1] 歪みテクスチャ (t1)
    rootParameters[1].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[1].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[1].DescriptorTable.pDescriptorRanges   = &distortionRange;
    rootParameters[1].DescriptorTable.NumDescriptorRanges = 1;

    desc.pParameters       = rootParameters;
    desc.NumParameters     = _countof(rootParameters);
    desc.pStaticSamplers   = staticSamplers_;
    desc.NumStaticSamplers = _countof(staticSamplers_);

    hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);
    if (FAILED(hr)) {
        Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
        assert(false);
    }

    hr = dxCommon_->GetDevice()->CreateRootSignature(
        0, signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(),
        IID_PPV_ARGS(&rootSignature_));
    assert(SUCCEEDED(hr));
}

///============================================================
/// グラフィックスパイプライン作成（基底クラスに委譲）
///============================================================
void Distortion::CreateGraphicsPipeline() {
    BasePostEffect::CreateGraphicsPipeline();
}

///============================================================
/// 描画ステート設定
///============================================================
void Distortion::SetDrawState(ID3D12GraphicsCommandList* commandList) {
    BasePostEffect::SetDrawState(commandList);
}

///============================================================
/// 定数バッファ作成（このエフェクトはパラメータ不要）
///============================================================
void Distortion::CreateConstantBuffer() {
    // 歪み強度はトレイル側の DistortionParam CB で制御するため不要
}

///============================================================
/// 描画
///============================================================
void Distortion::Draw(ID3D12GraphicsCommandList* commandList) {
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // [0] t0: シーンテクスチャ
    commandList->SetGraphicsRootDescriptorTable(
        0, dxCommon_->GetDxRenderTarget()->GetRenderTextureGPUSrvHandle());

    // [1] t1: 歪みテクスチャ（R16G16F）
    commandList->SetGraphicsRootDescriptorTable(1, distortionSrvHandle_);

    commandList->DrawInstanced(3, 1, 0, 0);
}

///============================================================
/// ImGuiデバッグ
///============================================================
void Distortion::DebugParamImGui() {
#ifdef _DEBUG
    // 歪み強度はトレイル側で調整するためここでは表示のみ
    ImGui::Text("Distortion PostEffect: active");
#endif
}
