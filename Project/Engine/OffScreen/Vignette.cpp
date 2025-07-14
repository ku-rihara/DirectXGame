#include "Dx/DirectXCommon.h"
#include "Vignette.h"

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