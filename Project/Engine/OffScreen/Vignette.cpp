#include "Vignette.h"
#include"Dx/DxRenderTarget.h"
#include "Dx/DirectXCommon.h"

void Vignette::Init(DirectXCommon* dxCommon) {

    vsName_ = L"resources/Shader/OffScreen/Fullscreen.VS.hlsl";
    psName_ = L"resources/Shader/OffScreen/Vignette.PS.hlsl";
    BaseOffScreen::Init(dxCommon);
}

void Vignette::CreateGraphicsPipeline() {
    BaseOffScreen::CreateGraphicsPipeline();
}

void Vignette::CreateRootSignature() {
    BaseOffScreen::CreateRootSignature();
}

void Vignette::SetDrawState(ID3D12GraphicsCommandList* commandList) {
    BaseOffScreen::SetDrawState(commandList);
}

void Vignette::CreateConstantBuffer() {

}
void Vignette::Draw([[maybe_unused]] ID3D12GraphicsCommandList* commandList) {

    // プリミティブトポロジーを設定
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // テクスチャリソースを設定
    commandList->SetGraphicsRootDescriptorTable(0, dxCommon_->GetDxRenderTarget()->GetRenderTextureGPUSrvHandle());

    commandList->DrawInstanced(3, 1, 0, 0);
}


void Vignette::DebugParamImGui() {
}