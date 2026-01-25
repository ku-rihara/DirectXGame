#include "NormalScreen.h"

using namespace KetaEngine;
#include"Base/Dx/DxRenderTarget.h"
#include"Base/Dx/DirectXCommon.h"


void NormalScreen::Init(DirectXCommon* dxCommon) {
    
    vsName_ = L"resources/Shader/PostEffect/Fullscreen.VS.hlsl";
    psName_ = L"resources/Shader/PostEffect/Fullscreen.PS.hlsl";
    BasePostEffect::Init(dxCommon);
}

void NormalScreen::CreateGraphicsPipeline() {
    BasePostEffect::CreateGraphicsPipeline();
}

void NormalScreen::CreateRootSignature() {
    BasePostEffect::CreateRootSignature();
}

void NormalScreen::SetDrawState(ID3D12GraphicsCommandList* commandList) {
    BasePostEffect::SetDrawState(commandList);
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
