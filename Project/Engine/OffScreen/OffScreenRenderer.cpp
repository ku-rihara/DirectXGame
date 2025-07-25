#include "OffScreenRenderer.h"
#include "Dx/DirectXCommon.h"

#include "BoxFilter.h"
#include "GaussianFilter.h"
#include "GrayScale.h"
#include "LuminanceBasedOutline.h"
#include "NormalScreen.h"
#include "Outline.h"
#include "RadialBlur.h"
#include "Vignette.h"
#include"RandomNoize.h"

#include <imgui.h>

OffScreenRenderer* OffScreenRenderer::GetInstance() {
    static OffScreenRenderer instance;
    return &instance;
}

void OffScreenRenderer::Init(DirectXCommon* dxCommon) {
    dxCommon_    = dxCommon;
    currentMode_ = OffScreenMode::NONE;

    // それぞれ生成
    effects_[static_cast<size_t>(OffScreenMode::NONE)]             = std::make_unique<NormalScreen>();
    effects_[static_cast<size_t>(OffScreenMode::GRAY)]             = std::make_unique<GrayScale>();
    effects_[static_cast<size_t>(OffScreenMode::VIGNETTE)]         = std::make_unique<Vignette>();
    effects_[static_cast<size_t>(OffScreenMode::GAUS)]             = std::make_unique<GaussianFilter>();
    effects_[static_cast<size_t>(OffScreenMode::BOXFILTER)]        = std::make_unique<BoxFilter>();
    effects_[static_cast<size_t>(OffScreenMode::RADIALBLUR)]       = std::make_unique<RadialBlur>();
    effects_[static_cast<size_t>(OffScreenMode::RANDOMNOIZE)]       = std::make_unique<RandomNoize>();
    effects_[static_cast<size_t>(OffScreenMode::OUTLINE)]          = std::make_unique<Outline>();
    effects_[static_cast<size_t>(OffScreenMode::LUMINANCEOUTLINE)] = std::make_unique<LuminanceBasedOutline>();

    // 初期化
    for (size_t i = 0; i < effects_.size(); ++i) {
        effects_[i]->Init(dxCommon_);
    }
}

void OffScreenRenderer::Draw(ID3D12GraphicsCommandList* commandList) {
    effects_[static_cast<size_t>(currentMode_)]->SetDrawState(commandList);
    // 各OffScreenコマンド
    effects_[static_cast<size_t>(currentMode_)]->Draw(commandList);
}

void OffScreenRenderer::DrawImGui() {
#ifdef _DEBUG

    if (ImGui::Begin("CopyImageRenderer")) {
        const char* modeNames[] = {"None", "Gray", "Vignette", "Gaus", "BoxFilter", "RadiauBlur","RandomNoize" ,"Outline", "LuminanceBasedOutline"};
        int mode                = static_cast<int>(currentMode_);
        if (ImGui::Combo("OffScreenMode", &mode, modeNames, IM_ARRAYSIZE(modeNames))) {
            currentMode_ = static_cast<OffScreenMode>(mode);
        }
    }
    effects_[static_cast<size_t>(currentMode_)]->DebugParamImGui();
    ImGui::End();
#endif
}

void OffScreenRenderer::SetViewProjection(const ViewProjection* viewProjection) {
    viewProjection_ = viewProjection;
    for (size_t i = 0; i < effects_.size(); ++i) {
        effects_[i]->SetViewProjection(viewProjection_);
    }
}