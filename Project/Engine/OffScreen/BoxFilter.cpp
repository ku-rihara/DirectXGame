#include "Dx/DirectXCommon.h"
#include"Dx/DxRenderTarget.h"
#include "BoxFilter.h"

void BoxFilter::Init(DirectXCommon* dxCommon) {

    vsName_ = L"resources/Shader/OffScreen/Fullscreen.VS.hlsl";
    psName_ = L"resources/Shader/OffScreen/BoxFilter.PS.hlsl";
    BaseOffScreen::Init(dxCommon);
}

void BoxFilter::CreateGraphicsPipeline() {
    BaseOffScreen::CreateGraphicsPipeline();
}

void BoxFilter::CreateRootSignature() {
    BaseOffScreen::CreateRootSignature();
}

void BoxFilter::SetDrawState(ID3D12GraphicsCommandList* commandList) {
    BaseOffScreen::SetDrawState(commandList);
}

void BoxFilter::CreateConstantBuffer() {
}

void BoxFilter::Draw([[maybe_unused]] ID3D12GraphicsCommandList* commandList) {
    // プリミティブトポロジーを設定
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // テクスチャリソースを設定
    commandList->SetGraphicsRootDescriptorTable(0, dxCommon_->GetDxRenderTarget()->GetRenderTextureGPUSrvHandle());

    commandList->DrawInstanced(3, 1, 0, 0);
}

void BoxFilter::DebugParamImGui() {

 }