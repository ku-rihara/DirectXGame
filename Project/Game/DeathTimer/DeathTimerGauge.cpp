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

    // 枠のスプライト
    frameSprite_.reset(KetaEngine::Sprite::Create("DeathGauge/DeathGaugeFrame.png", true));
    gaugeSprite_.reset(KetaEngine::Sprite::Create("DeathGauge/DeathGauge.png", true));
    gaugeIcon_.reset(KetaEngine::Sprite::Create("DeathGauge/PlayerDeathGaugeIcon.png", true));

    // 初期色設定
    UpdateGaugeColor();
}

void DeathTimerGauge::Update(float deltaTime) {
    // UVスクロール更新
    UpdateGaugeUV(deltaTime);
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
        ImGui::DragFloat("Danger Threshold", &dangerThreshold_, 0.01f,0.0f,1.0f);
        ImGui::DragFloat("Normal Threshold", &normalThreshold_, 0.01f,0.0f,1.0f);

        ImGui::SeparatorText("Gauge Colors");
        ImGui::ColorEdit4("Safe Color", &safeColor_.x);
        ImGui::ColorEdit4("Normal Color", &normalColor_.x);
        ImGui::ColorEdit4("Danger Color", &dangerColor_.x);


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
}