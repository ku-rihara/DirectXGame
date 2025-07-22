#include "NormalScreen.h"
#include"Dx/DxRenderTarget.h"
#include"Dx/DirectXCommon.h"


void NormalScreen::Init(DirectXCommon* dxCommon) {
    
    vsName_ = L"resources/Shader/OffScreen/Fullscreen.VS.hlsl";
    psName_ = L"resources/Shader/OffScreen/Fullscreen.PS.hlsl";
    BaseOffScreen::Init(dxCommon);
}

void NormalScreen::CreateGraphicsPipeline() {
    BaseOffScreen::CreateGraphicsPipeline();
}

void NormalScreen::CreateRootSignature() {
    BaseOffScreen::CreateRootSignature();
}

void NormalScreen::SetDrawState(ID3D12GraphicsCommandList* commandList) {
    BaseOffScreen::SetDrawState(commandList);
}

void NormalScreen::CreateConstantBuffer() {
}
void NormalScreen::Draw([[maybe_unused]] ID3D12GraphicsCommandList* commandList) {
    // プリミティブトポロジーを設定
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // テクスチャリソースを設定
    commandList->SetGraphicsRootDescriptorTable(0, dxCommon_->GetDxRenderTarget()->GetRenderTextureGPUSrvHandle());

    commandList->DrawInstanced(3, 1, 0, 0);
}


void NormalScreen::DebugParamImGui() {
}