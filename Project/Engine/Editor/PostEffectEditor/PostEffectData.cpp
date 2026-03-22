#include "PostEffectData.h"

using namespace KetaEngine;
#include"Frame/Frame.h"
#include "PostEffect/GaussianFilter.h"
#include "PostEffect/RadialBlur.h"
#include "PostEffect/Dissolve.h"
#include "PostEffect/Outline.h"
#include "PostEffect/LuminanceBasedOutline.h"
#include <imgui.h>

void PostEffectData::Init(const std::string& name, const std::string& categoryName) {
    BaseEffectData::Init(name, categoryName);
    folderPath_ = "PostEffect/" + categoryName + "/Dates";

    if (!globalParameter_->HasRegisters(groupName_)) {
        globalParameter_->CreateGroup(groupName_);
        RegisterParams();
        globalParameter_->SyncParamForGroup(groupName_);
    } else {
        GetParams();
    }

    InitParams();
}

void PostEffectData::InitParams() {
    paramEase_.SetAdaptValue(&easedParam_);
    paramEase_.SetStartValue(paramStart_);
    paramEase_.SetEndValue(paramEnd_);

    paramEase_.SetOnFinishCallback([this]() {
        PostEffectRenderer::GetInstance()->SetPostEffectMode(PostEffectMode::NONE);
        playState_ = PlayState::STOPPED;
    });

    playState_ = PlayState::STOPPED;
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
    globalParameter_->Regist(groupName_, "outlineWeightRate",   &outlineWeightRate_);

    // LuminanceOutline
    globalParameter_->Regist(groupName_, "luminanceWeightRate", &luminanceWeightRate_);

    // イージング
    globalParameter_->Regist(groupName_, "durationTime", &durationTime_);
    globalParameter_->Regist(groupName_, "paramStart",   &paramStart_);
    globalParameter_->Regist(groupName_, "paramEnd",     &paramEnd_);
    globalParameter_->Regist(groupName_, "easeType",     &easeType_);
}

void PostEffectData::GetParams() {
    postEffectModeIndex_  = globalParameter_->GetValue<int32_t>(groupName_, "postEffectMode");
    startTime_            = globalParameter_->GetValue<float>(groupName_, "startTime");
    gaussSigma_           = globalParameter_->GetValue<float>(groupName_, "gaussSigma");
    radialCenter_         = globalParameter_->GetValue<Vector2>(groupName_, "radialCenter");
    radialBlurWidth_      = globalParameter_->GetValue<float>(groupName_, "radialBlurWidth");
    dissolveThreshold_    = globalParameter_->GetValue<float>(groupName_, "dissolveThreshold");
    dissolveColor_        = globalParameter_->GetValue<Vector3>(groupName_, "dissolveColor");
    outlineWeightRate_    = globalParameter_->GetValue<float>(groupName_, "outlineWeightRate");
    luminanceWeightRate_  = globalParameter_->GetValue<float>(groupName_, "luminanceWeightRate");
    durationTime_         = globalParameter_->GetValue<float>(groupName_, "durationTime");
    paramStart_           = globalParameter_->GetValue<float>(groupName_, "paramStart");
    paramEnd_             = globalParameter_->GetValue<float>(groupName_, "paramEnd");
    easeType_             = globalParameter_->GetValue<int32_t>(groupName_, "easeType");
}

void PostEffectData::Play() {
    BaseEffectData::Play();
    currentTime_ = 0.0f;

    // イージング設定
    paramEase_.SetStartValue(paramStart_);
    paramEase_.SetEndValue(paramEnd_);
    paramEase_.SetMaxTime(durationTime_);
    paramEase_.SetType(static_cast<EasingType>(easeType_));
    paramEase_.Reset();
    easedParam_ = paramStart_;

    // startTime が 0 なら即時適用
    if (startTime_ <= 0.0f) {
        if (durationTime_ > 0.0f) {
            SetMainParam(paramStart_);
        }
        ApplyToRenderer();
        if (durationTime_ <= 0.0f) {
            playState_ = PlayState::STOPPED;
        }
    }
}

void PostEffectData::Update(float speedRate) {
    if (playState_ != PlayState::PLAYING) {
        return;
    }

    const float timeSpeed = speedRate * Frame::DeltaTime();

    currentTime_ += timeSpeed;

    // startTime 待機中
    if (currentTime_ < startTime_) {
        return;
    }

    if (durationTime_ > 0.0f) {
        // Easing<float> によるパラメータ更新
        paramEase_.Update(timeSpeed);
        SetMainParam(easedParam_);
        ApplyToRenderer();
    } else {
        // durationTime が 0 → 一度適用して終了
        ApplyToRenderer();
        playState_ = PlayState::STOPPED;
    }
}

void PostEffectData::Reset() {
    currentTime_ = 0.0f;
    easedParam_  = 0.0f;
    paramEase_.Reset();
    playState_ = PlayState::STOPPED;
}

void PostEffectData::SetMainParam(float value) {
    switch (GetPostEffectMode()) {
    case PostEffectMode::GAUS:             gaussSigma_          = value; break;
    case PostEffectMode::RADIALBLUR:       radialBlurWidth_     = value; break;
    case PostEffectMode::DISSOLVE:         dissolveThreshold_   = value; break;
    case PostEffectMode::OUTLINE:          outlineWeightRate_   = value; break;
    case PostEffectMode::LUMINANCEOUTLINE: luminanceWeightRate_ = value; break;
    default: break;
    }
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

    // イージングパラメータ
    ImGui::Separator();
    ImGui::Text("--- Easing ---");
    ImGui::DragFloat("Duration Time", &durationTime_, 0.01f, 0.0f, 30.0f);
    ImGui::DragFloat("Param Start",   &paramStart_,   0.001f);
    ImGui::DragFloat("Param End",     &paramEnd_,     0.001f);
    ImGuiEasingTypeSelector("Easing Type", easeType_);

    // プレビュー適用ボタン
    ImGui::Separator();
    if (ImGui::Button("Preview Apply")) {
        ApplyToRenderer();
    }

    ImGui::PopID();
#endif // _DEBUG
}
