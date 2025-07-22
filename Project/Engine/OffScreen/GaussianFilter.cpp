#include "Dx/DirectXCommon.h"
#include"Dx/DxRenderTarget.h"
#include "GaussianFilter.h"

void GaussianFilter::Init(DirectXCommon* dxCommon) {

    vsName_ = L"resources/Shader/OffScreen/Fullscreen.VS.hlsl";
    psName_ = L"resources/Shader/OffScreen/GaussianFilter.PS.hlsl";
    BaseOffScreen::Init(dxCommon);
}

void GaussianFilter::CreateGraphicsPipeline() {
    BaseOffScreen::CreateGraphicsPipeline();
}

void GaussianFilter::CreateRootSignature() {
    BaseOffScreen::CreateRootSignature();
}

void GaussianFilter::SetDrawState(ID3D12GraphicsCommandList* commandList) {
    BaseOffScreen::SetDrawState(commandList);
}

void GaussianFilter::CreateConstantBuffer() {
}
void GaussianFilter::Draw([[maybe_unused]] ID3D12GraphicsCommandList* commandList) {
    // プリミティブトポロジーを設定
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // テクスチャリソースを設定
    commandList->SetGraphicsRootDescriptorTable(0, dxCommon_->GetDxRenderTarget()->GetRenderTextureGPUSrvHandle());

    commandList->DrawInstanced(3, 1, 0, 0);
}


void GaussianFilter::DebugParamImGui() {
}