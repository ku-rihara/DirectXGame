#include "ComboUIController.h"
#include "Frame/Frame.h"
#include "base/TextureManager.h"
#include "Easing/EasingFunction.h"
// behavior
#include "Behavior/ComboCountUp.h"
#include "Behavior/ComboWait.h"
#include <cstdint>
#include <imgui.h>

void ComboUIController::Init() {

    // グローバルパラメータ
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    // Ui生成
    for (int32_t i = 0; i < comboSprites_.size(); ++i) {
        comboSprites_[i] = std::make_unique<ComboUI>();
        comboSprites_[i]->Init(static_cast<ComboUI::ComboDigit>(i));
    }

    // Hit スプライト生成
    hitSprite_.reset(KetaEngine::Sprite::Create("UI/HitUI.dds", true));
    if (hitSprite_) {
        hitSprite_->SetAnchorPoint({0.5f, 0.5f});
        hitSprite_->SetIsDraw(false);
    }
    hitScaleEasing_.Init(kHitEasingFile_);
    hitScaleEasing_.SetAdaptValue(&hitEaseScale_);

    // コンボチェインゲージ生成
    chainGauge_ = std::make_unique<ComboChainGauge>();
    chainGauge_->Init();

    ChangeBehavior(std::make_unique<ComboWait>(this));
}

void ComboUIController::Update(int32_t comboNum) {

    behavior_->Update();

    // 各桁のuvを更新
    for (int32_t i = 0; i < comboSprites_.size(); ++i) {
        // 基準座標 + 桁ごとのオフセットを適用
        Vector2 digitPosition = basePosition_ + Vector2(digitOffset_.x * i, digitOffset_.y * i);

        comboSprites_[i]->Update(baseScale_, alpha_, color_);
        comboSprites_[i]->CalculateNumber(comboNum);
        comboSprites_[i]->SetPosition(digitPosition);
        comboSprites_[i]->SetRotateY(rotateY_);
    }

    // Hit スプライト更新
    if (hitSprite_) {
        const bool isVisible = (comboNum >= 1);
        hitSprite_->SetIsDraw(isVisible);
        if (isVisible) {
            hitScaleEasing_.Update(KetaEngine::Frame::DeltaTime());
            hitSprite_->transform_.pos      = basePosition_ + hitSpriteOffset_;
            hitSprite_->transform_.scale    = baseScale_ * hitEaseScale_;
            hitSprite_->transform_.rotate.y = hitSpriteRotate_;
            hitSprite_->SetColor(color_);
            hitSprite_->SetAlpha(alpha_);
        }
    }
}

void ComboUIController::AlphaAdaptForTime(float comboTime, float comboMaxTime) {

    if (comboMaxTime <= 0.0f) {
        alpha_ = 0.0f;
        chainGauge_->Update(0.0f, 0.0f, 0.0f);
        return;
    }

    const float elapsed = std::clamp(comboMaxTime - comboTime, 0.0f, comboMaxTime);
    alpha_ = EaseInQuint(1.0f, 0.0f, elapsed, comboMaxTime);

    // ゲージ更新
    chainGauge_->Update(comboTime, comboMaxTime, alpha_);
}

///=========================================================
/// パラメータ調整
///==========================================================
void ComboUIController::AdjustParam() {
#if defined(_DEBUG) || defined(DEVELOPMENT)

    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::SeparatorText("Parameter"); // パラメータ

        // 基準座標の調整
        ImGui::DragFloat2("Base Position", &basePosition_.x, 0.1f);
        // 基準スケールの調整
        ImGui::DragFloat2("Base Scale", &baseScale_.x, 0.01f);
        // 桁間オフセットの調整
        ImGui::DragFloat2("Digit Offset", &digitOffset_.x, 0.1f);
        // Y軸回転
        ImGui::DragFloat("RotateY", &rotateY_, 0.01f);

        ImGui::SeparatorText("Color");
        ImGui::ColorEdit3("Tint Color", &color_.x);

        ImGui::SeparatorText("Hit Sprite");
        ImGui::DragFloat2("Hit Sprite Offset", &hitSpriteOffset_.x, 1.0f);
        ImGui::DragFloat("Hit Sprite Rotate", &hitSpriteRotate_, 0.01f);

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }

    for (int32_t i = 0; i < comboSprites_.size(); ++i) {
        comboSprites_[i]->AdjustParam();
    }

    chainGauge_->AdjustParam();

#endif
}

///=========================================================
/// パラメータBind
///==========================================================
void ComboUIController::RegisterParams() {
    globalParameter_->Regist(groupName_, "basePosition", &basePosition_);
    globalParameter_->Regist(groupName_, "baseScale", &baseScale_);
    globalParameter_->Regist(groupName_, "digitOffset", &digitOffset_);
    globalParameter_->Regist(groupName_, "rotateY", &rotateY_);
    globalParameter_->Regist(groupName_, "color", &color_);
    globalParameter_->Regist(groupName_, "hitSpriteOffset", &hitSpriteOffset_);
    globalParameter_->Regist(groupName_, "hitSpriteRotate", &hitSpriteRotate_);
}

void ComboUIController::ChangeBehavior(std::unique_ptr<BaseComboUIBehavior> behavior) {
    behavior_ = std::move(behavior);
}

void ComboUIController::ChangeCountUPAnimation() {
    ChangeBehavior(std::make_unique<ComboCountUP>(this));

    // Hit スケールイージングをリセット（ヒット演出）
    hitScaleEasing_.Reset();
}