#include "PostEffectRenderer.h"

using namespace KetaEngine;
#include "Dx/DirectXCommon.h"

#include "BoxFilter.h"
#include "Dissolve.h"
#include "GaussianFilter.h"
#include "GrayScale.h"
#include "LuminanceBasedOutline.h"
#include "NormalScreen.h"
#include "Outline.h"
#include "RadialBlur.h"
#include "RandomNoize.h"
#include "Vignette.h"

#include <imgui.h>

PostEffectRenderer* PostEffectRenderer::GetInstance() {
    static PostEffectRenderer instance;
    return &instance;
}

void PostEffectRenderer::Init(DirectXCommon* dxCommon) {
    dxCommon_    = dxCommon;
    currentMode_ = PostEffectMode::NONE;

    // それぞれ生成
    effects_[static_cast<size_t>(PostEffectMode::NONE)]             = std::make_unique<NormalScreen>();
    effects_[static_cast<size_t>(PostEffectMode::GRAY)]             = std::make_unique<GrayScale>();
    effects_[static_cast<size_t>(PostEffectMode::VIGNETTE)]         = std::make_unique<Vignette>();
    effects_[static_cast<size_t>(PostEffectMode::GAUS)]             = std::make_unique<GaussianFilter>();
    effects_[static_cast<size_t>(PostEffectMode::BOXFILTER)]        = std::make_unique<BoxFilter>();
    effects_[static_cast<size_t>(PostEffectMode::RADIALBLUR)]       = std::make_unique<RadialBlur>();
    effects_[static_cast<size_t>(PostEffectMode::RANDOMNOIZE)]      = std::make_unique<RandomNoize>();
    effects_[static_cast<size_t>(PostEffectMode::DISSOLVE)]         = std::make_unique<Dissolve>();
    effects_[static_cast<size_t>(PostEffectMode::OUTLINE)]          = std::make_unique<Outline>();
    effects_[static_cast<size_t>(PostEffectMode::LUMINANCEOUTLINE)] = std::make_unique<LuminanceBasedOutline>();

    // 初期化
    for (size_t i = 0; i < effects_.size(); ++i) {
        effects_[i]->Init(dxCommon_);
    }
}

void PostEffectRenderer::Draw(ID3D12GraphicsCommandList* commandList) {
    effects_[static_cast<size_t>(currentMode_)]->SetDrawState(commandList);
    // 各OffScreenコマンド
    effects_[static_cast<size_t>(currentMode_)]->Draw(commandList);
}

void PostEffectRenderer::DrawImGui() {
#ifdef _DEBUG

    if (ImGui::Begin("PostEffects")) {
        const char* modeNames[] = {"None", "Gray", "Vignette", "Gaus", "BoxFilter", "RadiauBlur", "RandomNoize", "Dissolve", "Outline", "LuminanceBasedOutline"};
        int mode                = static_cast<int>(currentMode_);
        if (ImGui::Combo("PostEffectMode", &mode, modeNames, IM_ARRAYSIZE(modeNames))) {
            currentMode_ = static_cast<PostEffectMode>(mode);
        }
    }
    effects_[static_cast<size_t>(currentMode_)]->DebugParamImGui();
    ImGui::End();
#endif
}

void PostEffectRenderer::SetViewProjection(const ViewProjection* viewProjection) {
    viewProjection_ = viewProjection;
    for (size_t i = 0; i < effects_.size(); ++i) {
        effects_[i]->SetViewProjection(viewProjection_);
    }
}

// 指定したモードの効果を取得
BasePostEffect* PostEffectRenderer::GetEffect(PostEffectMode mode) {
    size_t index = static_cast<size_t>(mode);
    if (index >= effects_.size()) {
        return nullptr;
    }
    return effects_[index].get();
}

