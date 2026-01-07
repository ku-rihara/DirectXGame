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

void DeathTimerGauge::SetTimer(float currentTimer, float maxTimer) {

    if (maxTimer > 0.0f) {
        timerRatio_ = currentTimer / maxTimer;
        timerRatio_ = std::clamp(timerRatio_, 0.0f, 1.0f);

        if (gaugeSprite_) {
            gaugeSprite_->SetGaugeRate(timerRatio_);
        }
    } else {
        timerRatio_ = 0.0f;
        if (gaugeSprite_) {
            gaugeSprite_->SetGaugeRate(0.0f);
        }
    }
}

///==========================================================
/// パラメータ調整
///==========================================================
void DeathTimerGauge::AdjustParam() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::DragFloat("UV Scroll Speed", &uvScrollSpeed_, 0.01f, 0.0f, 2.0f);

        // デバッグ用：ゲージレートを手動調整
        if (gaugeSprite_) {
            float debugRate = gaugeSprite_->GetGaugeRate();
            if (ImGui::SliderFloat("Debug Gauge Rate", &debugRate, 0.0f, 1.0f)) {
                gaugeSprite_->SetGaugeRate(debugRate);
            }
        }

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
#endif // _DEBUG
}

void DeathTimerGauge::RegisterParams() {
    globalParameter_->Regist(groupName_, "uvScrollSpeed", &uvScrollSpeed_);
}