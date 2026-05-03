#include "DeathTimerGauge.h"
#include "Frame/Frame.h"
#include <algorithm>
#include <imgui.h>

void DeathTimerGauge::Init() {

    // グローバルパラメータ
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    // スプライト初期化
    SpriteInit();

    // 初期色設定
    UpdateGaugeColor();
}

void DeathTimerGauge::SpriteInit() {

    // 枠のスプライト
    frameSprite_.reset(KetaEngine::Sprite::Create("DeathGauge/DeathGaugeFrame.dds", true));
    gaugeSprite_.reset(KetaEngine::Sprite::Create("DeathGauge/DeathGauge.dds", true));
    nameSprite_.reset(KetaEngine::Sprite::Create("DeathGauge/DeathGaugeName.dds", true));
    iconFrameSprite_.reset(KetaEngine::Sprite::Create("DeathGauge/DeathGaugeIconFrame.dds", true));

    // アイコン3種 (Safe=Happy, Normal=Normal, Danger=Angry)
    const char* iconTextures[static_cast<int32_t>(GaugeState::Count)] = {
        "DeathGauge/DeathGaugeIcon_Happy.dds",
        "DeathGauge/DeathGaugeIcon_Normal.dds",
        "DeathGauge/DeathGaugeIcon_Angry.dds",
    };
    for (int32_t i = 0; i < static_cast<int32_t>(GaugeState::Count); ++i) {
        gaugeIcons_[i].reset(KetaEngine::Sprite::Create(iconTextures[i], true));
        gaugeIcons_[i]->transform_.scale = Vector2::ZeroVector();
    }

    // 初期スケールを0
    frameSprite_->transform_.scale = Vector2::ZeroVector();
    gaugeSprite_->transform_.scale = Vector2::ZeroVector();

    heatBeat_.heatBeatEase.Init("UIHeatBeat.json");
    heatBeat_.heatBeatEase.SetAdaptValue(&gaugeIcons_[static_cast<int32_t>(currentState_)]->transform_.scale);
    heatBeat_.heatBeatEase.SetOnFinishCallback([this] {
        heatBeat_.heatBeatEase.Reset();
    });

    displayRatio_ = 0.0f;
    for (int32_t i = 0; i < kPercentDigitCount; ++i) {
        percentDigits_[i].Init("StressPercent" + std::to_string(i));
    }
}

void DeathTimerGauge::Update(float deltaTime) {
    // UVスクロール更新
    UpdateGaugeUV(deltaTime);

    // ゲージのイージング（displayRatio_をtimerRatio_へ滑らかに近づける）
    displayRatio_ += (timerRatio_ - displayRatio_) * std::min(1.0f, gaugeEaseSpeed_ * deltaTime);
    displayRatio_ = std::clamp(displayRatio_, 0.0f, 1.0f);

    // ゲージ表示更新
    if (gaugeSprite_) {
        gaugeSprite_->SetGaugeRate(displayRatio_);
    }
    UpdateGaugeColor();

    // %表示更新
    UpdatePercentDigits();

    // ゲージアイコンの鼓動
    float speedRate = heatBeat_.heatBeatSpeedRate[static_cast<int32_t>(currentState_)];
    heatBeat_.heatBeatEase.Update(deltaTime * speedRate);
}

void DeathTimerGauge::UpdateGaugeUV(float deltaTime) {
    // UVを右から左にスクロール
    uvOffset_ += uvScrollSpeed_ * deltaTime;

    // オフセットが1.0を超えたらリセット
    if (uvOffset_ >= 1.0f) {
        uvOffset_ = 0.0f;
    }

    // スプライトのUV位置を更新
    if (gaugeSprite_) {
        gaugeSprite_->SetUVPosition(Vector2(uvOffset_, 0.0f));
    }
}

void DeathTimerGauge::UpdateGaugeColor() {
    if (!gaugeSprite_) {
        return;
    }

    GaugeState newState = GaugeState::Safe;
    Vector4 targetColor = safeColor_;

    // timerRatio_はストレス比率
    if (timerRatio_ >= dangerThreshold_) {
        newState    = GaugeState::Danger;
        targetColor = dangerColor_;
    } else if (timerRatio_ >= normalThreshold_) {
        newState    = GaugeState::Normal;
        targetColor = normalColor_;
    } else {
        newState    = GaugeState::Safe;
        targetColor = safeColor_;
    }

    // 状態が変わった場合のみアイコンを切り替え
    if (newState != currentState_) {
        gaugeIcons_[static_cast<int32_t>(currentState_)]->transform_.scale = Vector2::ZeroVector();
        currentState_                                                      = newState;
        heatBeat_.heatBeatEase.SetAdaptValue(&gaugeIcons_[static_cast<int32_t>(currentState_)]->transform_.scale);
    }

    // 色を設定
    gaugeIcons_[static_cast<int32_t>(currentState_)]->SetColor(Vector3(targetColor.x, targetColor.y, targetColor.z));
    gaugeSprite_->SetColor(Vector3(targetColor.x, targetColor.y, targetColor.z));
    gaugeSprite_->SetAlpha(targetColor.w);
}

void DeathTimerGauge::SetTimer(float currentTimer, float maxTimer) {
    if (maxTimer > 0.0f) {
        timerRatio_ = std::clamp(currentTimer / maxTimer, 0.0f, 1.0f);
    } else {
        timerRatio_ = 0.0f;
    }
}

///==========================================================
/// パラメータ調整
///==========================================================
void DeathTimerGauge::AdjustParam() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::SeparatorText("Gauge Easing");
        ImGui::DragFloat("Ease Speed", &gaugeEaseSpeed_, 0.1f, 0.1f, 30.0f);
        ImGui::Text("timerRatio: %.3f  displayRatio: %.3f", timerRatio_, displayRatio_);

        ImGui::SeparatorText("Stress Percent UI");
        ImGui::DragFloat2("Percent Base Pos",     &percentBasePos_.x,      0.5f);
        ImGui::DragFloat2("Percent Base Scale",   &percentBaseScale_.x,    0.01f);
        ImGui::DragFloat2("Percent Digit Offset", &percentDigitOffset_.x,  0.5f);
        ImGui::Text("Display Percent: %d%%", static_cast<int32_t>(displayRatio_ * 100.0f));

        ImGui::SeparatorText("UV Scroll");
        ImGui::DragFloat("UV Scroll Speed", &uvScrollSpeed_, 0.01f, 0.0f, 2.0f);

        ImGui::SeparatorText("Color Thresholds");
        ImGui::DragFloat("Danger Threshold", &dangerThreshold_, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Normal Threshold", &normalThreshold_, 0.01f, 0.0f, 1.0f);

        ImGui::SeparatorText("Gauge Colors");
        ImGui::ColorEdit4("Safe Color", &safeColor_.x);
        ImGui::ColorEdit4("Normal Color", &normalColor_.x);
        ImGui::ColorEdit4("Danger Color", &dangerColor_.x);

        for (int32_t i = 0; i < heatBeat_.heatBeatSpeedRate.size(); ++i) {
            ImGui::DragFloat(("heatBeatSpeedRate" + std::to_string(i)).c_str(), &heatBeat_.heatBeatSpeedRate[i], 0.01f);
        }

        // 現在の状態表示
        const char* stateNames[] = {"Safe", "Normal", "Danger"};
        ImGui::Text("Current State: %s", stateNames[static_cast<int>(currentState_)]);

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
#endif // _DEBUG
}

void DeathTimerGauge::RegisterParams() {
    globalParameter_->Regist(groupName_, "uvScrollSpeed",       &uvScrollSpeed_);
    globalParameter_->Regist(groupName_, "dangerThreshold",     &dangerThreshold_);
    globalParameter_->Regist(groupName_, "normalThreshold",     &normalThreshold_);
    globalParameter_->Regist(groupName_, "safeColor",           &safeColor_);
    globalParameter_->Regist(groupName_, "normalColor",         &normalColor_);
    globalParameter_->Regist(groupName_, "dangerColor",         &dangerColor_);
    for (int32_t i = 0; i < static_cast<int32_t>(heatBeat_.heatBeatSpeedRate.size()); ++i) {
        globalParameter_->Regist(groupName_, "heatBeatSpeedRate" + std::to_string(i), &heatBeat_.heatBeatSpeedRate[i]);
    }
    globalParameter_->Regist(groupName_, "gaugeEaseSpeed",      &gaugeEaseSpeed_);
    globalParameter_->Regist(groupName_, "percentBasePos",      &percentBasePos_);
    globalParameter_->Regist(groupName_, "percentBaseScale",    &percentBaseScale_);
    globalParameter_->Regist(groupName_, "percentDigitOffset",  &percentDigitOffset_);
}

void DeathTimerGauge::PlayTimerRecoveryScaling() {
    if (frameSprite_) {
        frameSprite_->PlaySpriteEaseAnimation("TimerRecoveryGaugeScaling", "EnemyDeath");
    }
    if (gaugeSprite_) {
        gaugeSprite_->PlaySpriteEaseAnimation("TimerRecoveryGaugeScaling", "EnemyDeath");
    }
    if (gaugeIcons_[static_cast<int32_t>(currentState_)]) {
        gaugeIcons_[static_cast<int32_t>(currentState_)]->PlaySpriteEaseAnimation("TimerRecoveryGaugeScaling", "EnemyDeath");
    }
    if (nameSprite_) {
        nameSprite_->PlaySpriteEaseAnimation("TimerRecoveryGaugeScaling", "EnemyDeath");
    }
    if (iconFrameSprite_) {
        iconFrameSprite_->PlaySpriteEaseAnimation("TimerRecoveryGaugeScaling", "EnemyDeath");
    }
    if (bombScrollSprite_) {
        bombScrollSprite_->PlaySpriteEaseAnimation("TimerRecoveryGaugeScaling", "EnemyDeath");
    }
}

void DeathTimerGauge::UpdatePercentDigits() {
    int32_t percent = std::clamp(static_cast<int32_t>(displayRatio_ * 100.0f), 0, 100);

    std::array<int32_t, kPercentDigitCount> digits  = {percent / 100, (percent / 10) % 10, percent % 10};
    std::array<bool,    kPercentDigitCount> visible  = {percent >= 100, percent >= 10, true};

    for (int32_t i = 0; i < kPercentDigitCount; ++i) {
        Vector2 pos = percentBasePos_ + Vector2(percentDigitOffset_.x * static_cast<float>(i),
                                                percentDigitOffset_.y * static_cast<float>(i));
        Vector2 scaledScale = {percentBaseScale_.x * percentSpriteMultiplier_.x,
                               percentBaseScale_.y * percentSpriteMultiplier_.y};
        percentDigits_[i].Update(digits[i], pos, scaledScale, 1.0f, visible[i]);
    }
}

void DeathTimerGauge::SetSpriteScales(const Vector2& scale) {
    frameSprite_->transform_.scale                                     = scale;
    gaugeSprite_->transform_.scale                                     = scale;
    gaugeIcons_[static_cast<int32_t>(currentState_)]->transform_.scale = scale;
    nameSprite_->transform_.scale                                      = scale;
    iconFrameSprite_->transform_.scale                                 = scale;
    percentSpriteMultiplier_                                           = scale;
}