#include "GrayScale.h"
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