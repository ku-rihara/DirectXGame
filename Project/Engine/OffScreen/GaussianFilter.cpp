#include "Dx/DirectXCommon.h"
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
void GaussianFilter::SetCommand([[maybe_unused]] ID3D12GraphicsCommandList* commandList) {
}


void GaussianFilter::DebugParamImGui() {
}