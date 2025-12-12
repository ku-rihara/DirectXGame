#include "GrayScale.h"

using namespace KetaEngine;
#include"Dx/DxRenderTarget.h"
#include "Dx/DirectXCommon.h"

void GrayScale::Init(DirectXCommon* dxCommon) {

    vsName_ = L"resources/Shader/PostEffect/Fullscreen.VS.hlsl";
    psName_ = L"resources/Shader/PostEffect/Grayscale.PS.hlsl";
    BasePostEffect::Init(dxCommon);
}

void GrayScale::CreateGraphicsPipeline() {
    BasePostEffect::CreateGraphicsPipeline();
}

void GrayScale::CreateRootSignature() {
    BasePostEffect::CreateRootSignature();
}

void GrayScale::SetDrawState(ID3D12GraphicsCommandList* commandList) {
    BasePostEffect::SetDrawState(commandList);
}

void GrayScale::CreateConstantBuffer() {
}
void GrayScale::Draw([[maybe_unused]] ID3D12GraphicsCommandList* commandList) {
    // プリミティブトポロジーを設定
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // テクスチャリソースを設定
    commandList->SetGraphicsRootDescriptorTable(0, dxCommon_->GetDxRenderTarget()->GetRenderTextureGPUSrvHandle());

    commandList->DrawInstanced(3, 1, 0, 0);
}


void GrayScale::DebugParamImGui() {
}
