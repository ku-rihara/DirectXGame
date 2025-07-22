#include "Vignette.h"
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
void Vignette::SetCommand([[maybe_unused]] ID3D12GraphicsCommandList* commandList) {
}


void Vignette::DebugParamImGui() {
}