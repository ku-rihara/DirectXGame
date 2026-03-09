#include "PostEffectData.h"

using namespace KetaEngine;
#include "PostEffect/GaussianFilter.h"
#include "PostEffect/RadialBlur.h"
#include "PostEffect/Dissolve.h"
#include "PostEffect/Outline.h"
#include "PostEffect/LuminanceBasedOutline.h"
#include <imgui.h>

void PostEffectData::Init(const std::string& name, const std::string& categoryName) {
    BaseEffectData::Init(name, categoryName);
    folderPath_ = "PostEffectEditor/" + categoryName + "/Dates";

    globalParameter_->CreateGroup(groupName_);
    InitParams();
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);
}

void PostEffectData::InitParams() {
    // デフォルト値はメンバ変数の初期値で管理
}

void PostEffectData::RegisterParams() {
    // 共通
    globalParameter_->Regist(groupName_, "postEffectMode", &postEffectModeIndex_);
    globalParameter_->Regist(groupName_, "startTime",      &startTime_);

    // GaussianFilter
    globalParameter_->Regist(groupName_, "gaussSigma",         &gaussSigma_);

    // RadialBlur
    globalParameter_->Regist(groupName_, "radialCenter",     &radialCenter_);
    globalParameter_->Regist(groupName_, "radialBlurWidth",  &radialBlurWidth_);

    // Dissolve
    globalParameter_->Regist(groupName_, "dissolveThreshold", &dissolveThreshold_);
    globalParameter_->Regist(groupName_, "dissolveColor",     &dissolveColor_);

    // Outline
    globalParameter_->Regist(groupName_, "outlineWeightRate",      &outlineWeightRate_);

    // LuminanceOutline
    globalParameter_->Regist(groupName_, "luminanceWeightRate",    &luminanceWeightRate_);
}

void PostEffectData::GetParams() {
  
}

void PostEffectData::Play() {
    BaseEffectData::Play();
    currentTime_ = 0.0f;

    // startTime が 0 ならば即時適用
    if (startTime_ <= 0.0f) {
        ApplyToRenderer();
        playState_ = PlayState::STOPPED;
    }
}

void PostEffectData::Update(float speedRate) {
    if (playState_ != PlayState::PLAYING) {
        return;
    }

    currentTime_ += speedRate;

    if (currentTime_ >= startTime_) {
        ApplyToRenderer();
        playState_ = PlayState::STOPPED;
    }
}

void PostEffectData::Reset() {
    currentTime_ = 0.0f;
    playState_   = PlayState::STOPPED;
}

void PostEffectData::ApplyToRenderer() {
    auto* renderer = PostEffectRenderer::GetInstance();
    renderer->SetPostEffectMode(GetPostEffectMode());

    switch (GetPostEffectMode()) {
    case PostEffectMode::GAUS:
        if (auto* effect = renderer->GetEffect<GaussianFilter>(PostEffectMode::GAUS)) {
            effect->SetSigma(gaussSigma_);
        }
        break;

    case PostEffectMode::RADIALBLUR:
        if (auto* effect = renderer->GetEffect<RadialBlur>(PostEffectMode::RADIALBLUR)) {
            effect->SetCenter(radialCenter_);
            effect->SetBlurWidth(radialBlurWidth_);
        }
        break;

    case PostEffectMode::DISSOLVE:
        if (auto* effect = renderer->GetEffect<Dissolve>(PostEffectMode::DISSOLVE)) {
            effect->SetThreshold(dissolveThreshold_);
            effect->SetColor(dissolveColor_);
        }
        break;

    case PostEffectMode::OUTLINE:
        if (auto* effect = renderer->GetEffect<Outline>(PostEffectMode::OUTLINE)) {
            effect->SetWeightRate(outlineWeightRate_);
        }
        break;

    case PostEffectMode::LUMINANCEOUTLINE:
        if (auto* effect = renderer->GetEffect<LuminanceBasedOutline>(PostEffectMode::LUMINANCEOUTLINE)) {
            effect->SetWeightRate(luminanceWeightRate_);
        }
        break;

    default:
        break;
    }
}

void PostEffectData::AdjustParam() {
#ifdef _DEBUG
    ImGui::SeparatorText(("PostEffect: " + groupName_).c_str());
    ImGui::PushID(groupName_.c_str());

    // エフェクトタイプ選択
    static const char* kModeNames[] = {
        "None", "Gray", "Vignette", "Gaussian", "BoxFilter",
        "RadialBlur", "RandomNoise", "Dissolve", "Outline", "LuminanceOutline"
    };
    ImGui::Combo("Effect Type", &postEffectModeIndex_, kModeNames, IM_ARRAYSIZE(kModeNames));

    // 共通パラメータ
    ImGui::DragFloat("Start Time", &startTime_, 0.01f, 0.0f, 60.0f);

    // モード固有パラメータ
    ImGui::Separator();
    switch (GetPostEffectMode()) {
    case PostEffectMode::NONE:
    case PostEffectMode::GRAY:
    case PostEffectMode::VIGNETTE:
    case PostEffectMode::BOXFILTER:
    case PostEffectMode::RANDOMNOISE:
        ImGui::TextDisabled("(固有パラメータなし)");
        break;

    case PostEffectMode::GAUS:
        ImGui::DragFloat("Sigma", &gaussSigma_, 0.01f, 0.1f, 20.0f);
        break;

    case PostEffectMode::RADIALBLUR:
        ImGui::DragFloat2("Center", &radialCenter_.x, 0.001f, 0.0f, 1.0f);
        ImGui::DragFloat("Blur Width", &radialBlurWidth_, 0.001f, 0.0f, 1.0f);
        break;

    case PostEffectMode::DISSOLVE:
        ImGui::DragFloat("Threshold", &dissolveThreshold_, 0.01f, 0.0f, 1.0f);
        ImGui::ColorEdit3("Edge Color", &dissolveColor_.x);
        break;

    case PostEffectMode::OUTLINE:
        ImGui::DragFloat("Weight Rate", &outlineWeightRate_, 0.01f, 0.0f, 1.0f);
        break;

    case PostEffectMode::LUMINANCEOUTLINE:
        ImGui::DragFloat("Weight Rate", &luminanceWeightRate_, 0.01f, 0.0f, 1.0f);
        break;

    default:
        break;
    }

    // プレビュー適用ボタン
    ImGui::Separator();
    if (ImGui::Button("Preview Apply")) {
        ApplyToRenderer();
    }

    ImGui::PopID();
#endif // _DEBUG
}
