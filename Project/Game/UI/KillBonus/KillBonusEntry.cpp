#include "KillBonusEntry.h"

void KillBonusEntry::Init(int32_t comboMultiplier, bool hasSimKill, int32_t simKillBonusValue, const Vector2& spawnPos) {
    comboMultiplier_   = comboMultiplier;
    hasSimKill_        = hasSimKill;
    simKillBonusValue_ = simKillBonusValue;
    currentPos_        = spawnPos;
    alpha_             = 1.0f;
    fadeTimer_         = 0.0f;

    recoverySprite_.reset(KetaEngine::Sprite::Create("UI/KillRecovery.dds", true));
    if (recoverySprite_) {
        recoverySprite_->SetAnchorPoint({0.5f, 0.5f});
    }

    comboDigit_.Init();

    comboLabelSprite_.reset(KetaEngine::Sprite::Create("UI/KillComboBonus.dds", true));
    if (comboLabelSprite_) {
        comboLabelSprite_->SetAnchorPoint({0.5f, 0.5f});
    }

    for (auto& d : simKillDigits_) {
        d.Init();
    }

    simKillLabelSprite_.reset(KetaEngine::Sprite::Create("UI/SimKillBonus.dds", true));
    if (simKillLabelSprite_) {
        simKillLabelSprite_->SetAnchorPoint({0.5f, 0.5f});
        simKillLabelSprite_->SetIsDraw(hasSimKill_);
    }

    posEasing_.Init("KillBonusPosShift");
    posEasing_.SetAdaptValue(&currentPos_);

    spawnScale_ = 0.0f;
    spawnEasing_.Init("KillBonusSpawn");
    spawnEasing_.SetAdaptValue(&spawnScale_);
    spawnEasing_.Reset();
}

void KillBonusEntry::Update(float deltaTime, const KillBonusLayoutParam& layout) {
    // アルファフェード
    fadeTimer_ += deltaTime;
    if (layout.fadeDuration > 0.0f) {
        alpha_ = 1.0f - (fadeTimer_ / layout.fadeDuration);
        if (alpha_ < 0.0f) {
            alpha_ = 0.0f;
        }
    }

    spawnEasing_.Update(deltaTime);
    posEasing_.Update(deltaTime);

    // 全要素にかけるスケール（スポーンアニメ × アルファ）
    const Vector2 scaledDigit = layout.digitScale * spawnScale_;

    // 回復スプライト
    if (recoverySprite_) {
        recoverySprite_->transform_.pos   = currentPos_ + layout.recoveryOffset;
        recoverySprite_->transform_.scale = Vector2(spawnScale_, spawnScale_);
        recoverySprite_->SetAlpha(alpha_);
    }

    // コンボ倍率桁
    comboDigit_.Update(comboMultiplier_, currentPos_ + layout.comboOffset, scaledDigit, alpha_);

    // コンボラベル
    if (comboLabelSprite_) {
        comboLabelSprite_->transform_.pos   = currentPos_ + layout.comboLabelOffset;
        comboLabelSprite_->transform_.scale = Vector2(spawnScale_, spawnScale_);
        comboLabelSprite_->SetAlpha(alpha_);
    }

    // シムキルボーナス（hasSimKill_のときのみ表示）
    const bool showSim = hasSimKill_;
    int32_t ones = simKillBonusValue_ % 10;
    int32_t tens = (simKillBonusValue_ / 10) % 10;

    simKillDigits_[0].Update(ones, currentPos_ + layout.simKillOffset,                    scaledDigit, alpha_, showSim);
    simKillDigits_[1].Update(tens, currentPos_ + layout.simKillOffset + layout.digitSpacing, scaledDigit, alpha_, showSim && (simKillBonusValue_ >= 10));

    if (simKillLabelSprite_) {
        simKillLabelSprite_->transform_.pos   = currentPos_ + layout.simKillLabelOffset;
        simKillLabelSprite_->transform_.scale = Vector2(spawnScale_, spawnScale_);
        simKillLabelSprite_->SetAlpha(showSim ? alpha_ : 0.0f);
        simKillLabelSprite_->SetIsDraw(showSim);
    }
}

void KillBonusEntry::ShiftTo(const Vector2& newTarget) {
    posEasing_.SetStartValue(currentPos_);
    posEasing_.SetEndValue(newTarget);
    posEasing_.Reset();
}

void KillBonusEntry::SetSimKill(int32_t bonusValue) {
    hasSimKill_        = true;
    simKillBonusValue_ = bonusValue;
    if (simKillLabelSprite_) {
        simKillLabelSprite_->SetIsDraw(true);
    }
}
