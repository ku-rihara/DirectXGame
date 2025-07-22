#include "NormalScreen.h"
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
void NormalScreen::SetCommand([[maybe_unused]] ID3D12GraphicsCommandList* commandList) {
}


void NormalScreen::DebugParamImGui() {
}