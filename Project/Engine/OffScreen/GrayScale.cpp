#include "GrayScale.h"
#include"Dx/DxRenderTarget.h"
#include "Dx/DirectXCommon.h"

void GrayScale::Init(DirectXCommon* dxCommon) {

    vsName_ = L"resources/Shader/OffScreen/Fullscreen.VS.hlsl";
    psName_ = L"resources/Shader/OffScreen/Grayscale.PS.hlsl";
    BaseOffScreen::Init(dxCommon);
}

void GrayScale::CreateGraphicsPipeline() {
    BaseOffScreen::CreateGraphicsPipeline();
}

void GrayScale::CreateRootSignature() {
    BaseOffScreen::CreateRootSignature();
}

void GrayScale::SetDrawState(ID3D12GraphicsCommandList* commandList) {
    BaseOffScreen::SetDrawState(commandList);
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