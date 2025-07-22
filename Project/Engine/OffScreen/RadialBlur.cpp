#include "Dx/DirectXCommon.h"
#include"Dx/DxRenderTarget.h"
#include "RadialBlur.h"

void RadialBlur::Init(DirectXCommon* dxCommon) {

    vsName_ = L"resources/Shader/OffScreen/Fullscreen.VS.hlsl";
    psName_ = L"resources/Shader/OffScreen/RadialBlur.PS.hlsl";
    BaseOffScreen::Init(dxCommon);
}

void RadialBlur::CreateGraphicsPipeline() {
    BaseOffScreen::CreateGraphicsPipeline();
}

void RadialBlur::CreateRootSignature() {
    BaseOffScreen::CreateRootSignature();
}

void RadialBlur::SetDrawState(ID3D12GraphicsCommandList* commandList) {
    BaseOffScreen::SetDrawState(commandList);
}

void RadialBlur::CreateConstantBuffer() {
}
void RadialBlur::Draw([[maybe_unused]] ID3D12GraphicsCommandList* commandList) {

      // プリミティブトポロジーを設定
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // テクスチャリソースを設定
    commandList->SetGraphicsRootDescriptorTable(0, dxCommon_->GetDxRenderTarget()->GetRenderTextureGPUSrvHandle());

    commandList->DrawInstanced(3, 1, 0, 0);
}


void RadialBlur::DebugParamImGui() {
}