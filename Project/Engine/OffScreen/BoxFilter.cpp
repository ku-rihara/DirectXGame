#include "Dx/DirectXCommon.h"
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
void BoxFilter::SetCommand([[maybe_unused]] ID3D12GraphicsCommandList* commandList) {

}

void BoxFilter::DebugParamImGui() {

 }