#include "Dx/DirectXCommon.h"
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