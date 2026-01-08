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
    frameSprite_.reset(KetaEngine::Sprite::Create("DeathGauge/DeathGaugeFrame.png", true));
    gaugeSprite_.reset(KetaEngine::Sprite::Create("DeathGauge/DeathGauge.png", true));
    gaugeIcon_.reset(KetaEngine::Sprite::Create("DeathGauge/PlayerDeathGaugeIcon.png", true));

    frameSprite_->transform_.scale = Vector2::ZeroVector();
    gaugeSprite_->transform_.scale = Vector2::ZeroVector();
    gaugeIcon_->transform_.scale   = Vector2::ZeroVector();

    heatBeat_.heatBeatEase.Init("UIHeatBeat.json");
    heatBeat_.heatBeatEase.SetAdaptValue(&gaugeIcon_->transform_.scale);
    heatBeat_.heatBeatEase.SetOnFinishCallback([this] {
        heatBeat_.heatBeatEase.Reset();
    });
}

void DeathTimerGauge::Update(float deltaTime) {
    // UVスクロール更新
    UpdateGaugeUV(deltaTime);

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
        gaugeSprite_->uvTransform_.pos.x = uvOffset_;
    }
}

void DeathTimerGauge::UpdateGaugeColor() {
    if (!gaugeSprite_) {
        return;
    }

    GaugeState newState = GaugeState::Safe;
    Vector4 targetColor = safeColor_;

    // timerRatio_に基づいて状態を判定
    if (timerRatio_ <= dangerThreshold_) {
        newState    = GaugeState::Danger;
        targetColor = dangerColor_;
    } else if (timerRatio_ <= normalThreshold_) {
        newState    = GaugeState::Normal;
        targetColor = normalColor_;
    } else {
        newState    = GaugeState::Safe;
        targetColor = safeColor_;
    }

    // 状態が変わった場合のみ色を更新
    if (newState != currentState_) {
        currentState_ = newState;
    }

    // 色を設定
    gaugeSprite_->SetColor(Vector3(targetColor.x, targetColor.y, targetColor.z));
    gaugeSprite_->SetAlpha(targetColor.w);
}

void DeathTimerGauge::SetTimer(float currentTimer, float maxTimer) {

    if (maxTimer > 0.0f) {
        timerRatio_ = currentTimer / maxTimer;
        timerRatio_ = std::clamp(timerRatio_, 0.0f, 1.0f);

        if (gaugeSprite_) {
            gaugeSprite_->SetGaugeRate(timerRatio_);
        }

        // 色更新
        UpdateGaugeColor();
    } else {
        timerRatio_ = 0.0f;
        if (gaugeSprite_) {
            gaugeSprite_->SetGaugeRate(0.0f);
        }
        UpdateGaugeColor();
    }
}

///==========================================================
/// パラメータ調整
///==========================================================
void DeathTimerGauge::AdjustParam() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

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
    globalParameter_->Regist(groupName_, "uvScrollSpeed", &uvScrollSpeed_);
    globalParameter_->Regist(groupName_, "dangerThreshold", &dangerThreshold_);
    globalParameter_->Regist(groupName_, "normalThreshold", &normalThreshold_);
    globalParameter_->Regist(groupName_, "safeColor", &safeColor_);
    globalParameter_->Regist(groupName_, "normalColor", &normalColor_);
    globalParameter_->Regist(groupName_, "dangerColor", &dangerColor_);
    for (int32_t i = 0; i < heatBeat_.heatBeatSpeedRate.size(); ++i) {
        globalParameter_->Regist(groupName_, "heatBeatSpeedRate" + std::to_string(i), &heatBeat_.heatBeatSpeedRate[i]);
    }
}

void DeathTimerGauge::SetSpriteScales(const Vector2& scale) {
    frameSprite_->transform_.scale = scale;
    gaugeSprite_->transform_.scale = scale;
    gaugeIcon_->transform_.scale   = scale;
}