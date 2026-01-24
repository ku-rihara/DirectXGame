#include "Base/Dx/DirectXCommon.h"
#include"Base/Dx/DxRenderTarget.h"
#include "BoxFilter.h"

using namespace KetaEngine;

void BoxFilter::Init(DirectXCommon* dxCommon) {

    vsName_ = L"resources/Shader/PostEffect/Fullscreen.VS.hlsl";
    psName_ = L"resources/Shader/PostEffect/BoxFilter.PS.hlsl";
    BasePostEffect::Init(dxCommon);
}

void BoxFilter::CreateGraphicsPipeline() {
    BasePostEffect::CreateGraphicsPipeline();
}

void BoxFilter::CreateRootSignature() {
    BasePostEffect::CreateRootSignature();
}

void BoxFilter::SetDrawState(ID3D12GraphicsCommandList* commandList) {
    BasePostEffect::SetDrawState(commandList);
}

void BoxFilter::CreateConstantBuffer() {
}

void BoxFilter::Draw([[maybe_unused]] ID3D12GraphicsCommandList* commandList) {
    // プリミティブトポロジーを設定
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // テクスチャリソースを設定
    commandList->SetGraphicsRootDescriptorTable(0, dxCommon_->GetDxRenderTarget()->GetRenderTextureGPUSrvHandle());

    commandList->DrawInstanced(3, 1, 0, 0);
}

void BoxFilter::DebugParamImGui() {

 }
