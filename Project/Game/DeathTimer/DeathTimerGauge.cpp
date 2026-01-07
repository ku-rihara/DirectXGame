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
    frameSprite_.reset(KetaEngine::Sprite::Create("DeathGaugeFrame.png", true));
    gaugeSprite_.reset(KetaEngine::Sprite::Create("DeathGauge.png", true));
    gaugeIcon_.reset(KetaEngine::Sprite::Create("PlayerDeathGaugeIcon.png", true));
    // 爆弾スクロールスプライト
    bombScrollSprite_.reset(KetaEngine::Sprite::Create("DeathTimerBombScroll.png", true));
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
    if (bombScrollSprite_) {
        bombScrollSprite_->uvTransform_.pos.x = uvOffset_;
    }
}

void DeathTimerGauge::SetTimer(float currentTimer, float maxTimer) {
    if (maxTimer > 0.0f) {
        timerRatio_ = currentTimer / maxTimer;
        timerRatio_ = std::clamp(timerRatio_, 0.0f, 1.0f);
    } else {
        timerRatio_ = 0.0f;
    }
}

///==========================================================
/// パラメータ調整
///==========================================================
void DeathTimerGauge::AdjustParam() {

#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::DragFloat2("Gauge Position", &gaugePosition_.x, 1.0f);
        ImGui::DragFloat2("Gauge Scale", &gaugeScale_.x, 0.01f);
        ImGui::DragFloat("UV Scroll Speed", &uvScrollSpeed_, 0.01f, 0.0f, 2.0f);
        ImGui::DragFloat("Timer Ratio (Debug)", &timerRatio_, 0.01f, 0.0f, 1.0f);

        // 位置とスケールを即座に反映
        if (frameSprite_) {
            frameSprite_->transform_.pos   = gaugePosition_;
            frameSprite_->transform_.scale = gaugeScale_;
        }
        if (bombScrollSprite_) {
            bombScrollSprite_->transform_.pos = {gaugePosition_.x + 15.0f, gaugePosition_.y + 25.0f};
        }

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }

#endif // _DEBUG
}

void DeathTimerGauge::RegisterParams() {
    globalParameter_->Regist(groupName_, "gaugePosition", &gaugePosition_);
    globalParameter_->Regist(groupName_, "gaugeScale", &gaugeScale_);
    globalParameter_->Regist(groupName_, "uvScrollSpeed", &uvScrollSpeed_);
}