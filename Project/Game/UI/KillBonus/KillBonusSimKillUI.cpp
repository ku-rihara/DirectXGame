#include "KillBonusSimKillUI.h"
#include "Vector3.h"
#include <cmath>

void KillBonusSimKillUI::Init() {
    simKillLabelSprite_.reset(KetaEngine::Sprite::Create("UI/SimKillBonus.dds", false));
    if (simKillLabelSprite_) {
        simKillLabelSprite_->SetAnchorPoint({0.5f, 0.5f});
    }

    bodySprite_.reset(KetaEngine::Sprite::Create("UI/SimKillBody.dds", false));
    if (bodySprite_) {
        bodySprite_->SetAnchorPoint({0.5f, 0.5f});
    }

    leftParenSprite_.reset(KetaEngine::Sprite::Create("UI/ALeftParenthesis.dds", false));
    if (leftParenSprite_) {
        leftParenSprite_->SetAnchorPoint({0.5f, 0.5f});
    }

    rightParenSprite_.reset(KetaEngine::Sprite::Create("UI/ARightParenthesis.dds", false));
    if (rightParenSprite_) {
        rightParenSprite_->SetAnchorPoint({0.5f, 0.5f});
    }

    multiplyIconSprite_.reset(KetaEngine::Sprite::Create("UI/MultiplyIcon.dds", false));
    if (multiplyIconSprite_) {
        multiplyIconSprite_->SetAnchorPoint({0.5f, 0.5f});
    }

    comboDecimalPointSprite_.reset(KetaEngine::Sprite::Create("UI/DecimalPoint.dds", false));
    if (comboDecimalPointSprite_) {
        comboDecimalPointSprite_->SetAnchorPoint({0.5f, 0.5f});
    }

    killCountDigits_[0].Init("SimKill_1");
    killCountDigits_[1].Init("SimKill_10");
    comboIntDigit_.Init("SimKillComboInt");
    comboDecimalDigits_[0].Init("SimKillComboDecimal_1");
    comboDecimalDigits_[1].Init("SimKillComboDecimal_2");

    spawnEasing_.Init("KillBonusSpawn.json");
    spawnEasing_.SetAdaptValue(&currentScale_);

    closeEasing_.Init("KillBonusClose.json");
    closeEasing_.SetAdaptValue(&currentScale_);

    SetAllVisible(false);
    state_ = State::Hidden;
}

void KillBonusSimKillUI::Spawn(int32_t killCount, float comboBonusValue) {
    killCount_       = killCount;
    comboBonusValue_ = comboBonusValue;
    currentScale_    = 0.0f;
    spawnEasing_.Reset();
    state_ = State::Spawning;
    SetAllVisible(true);
}

void KillBonusSimKillUI::Close() {
    if (state_ == State::Hidden) {
        return;
    }
    closeEasing_.SetStartValue(currentScale_);
    closeEasing_.Reset();
    state_ = State::Closing;
}

void KillBonusSimKillUI::Update(float deltaTime, const KillBonusSimKillUILayout& layout) {
    switch (state_) {
    case State::Spawning:
        spawnEasing_.Update(deltaTime);
        if (spawnEasing_.IsFinished()) {
            currentScale_ = 1.0f;
            state_        = State::Displaying;
        }
        break;
    case State::Closing:
        closeEasing_.Update(deltaTime);
        if (closeEasing_.IsFinished()) {
            currentScale_ = 0.0f;
            state_        = State::Hidden;
            SetAllVisible(false);
        }
        break;
    default:
        break;
    }

    if (state_ == State::Hidden) {
        return;
    }

    const Vector2  scaledDigit  = layout.digitScale * currentScale_;
    const Vector2  scaledCombo  = layout.comboDigitScale * currentScale_;
    const Vector2& pos          = layout.basePos;
    const Vector4  tierColor    = GetKillBonusTierColor(killCount_, layout.colorTiers);

    // ラベル
    if (simKillLabelSprite_) {
        simKillLabelSprite_->SetIsDraw(true);
        simKillLabelSprite_->transform_.pos   = pos + layout.labelOffset;
        simKillLabelSprite_->transform_.scale = Vector2(currentScale_, currentScale_);
        simKillLabelSprite_->SetAlpha(currentScale_);
    }

    // キル数（1の位・10の位）段階カラー適用
    const int32_t ones    = killCount_ % 10;
    const int32_t tens    = (killCount_ / 10) % 10;
    const Vector3 tierRGB = {tierColor.x, tierColor.y, tierColor.z};

    killCountDigits_[0].SetColor(tierRGB);
    killCountDigits_[1].SetColor(tierRGB);
    killCountDigits_[0].Update(ones, pos + layout.digitOffset,                       scaledDigit, tierColor.w * currentScale_);
    killCountDigits_[1].Update(tens, pos + layout.digitOffset + layout.digitSpacing, scaledDigit, tierColor.w * currentScale_, killCount_ >= 10 || showMaxDigits_);

    // MultiplyIcon
    if (multiplyIconSprite_) {
        multiplyIconSprite_->SetIsDraw(true);
        multiplyIconSprite_->transform_.pos   = pos + layout.multiplyIconOffset;
        multiplyIconSprite_->transform_.scale = Vector2(currentScale_, currentScale_);
        multiplyIconSprite_->SetAlpha(currentScale_);
    }

    // 体スプライト（段階カラー適用）
    if (bodySprite_) {
        bodySprite_->SetIsDraw(true);
        bodySprite_->transform_.pos   = pos + layout.bodyOffset;
        bodySprite_->transform_.scale = Vector2(currentScale_, currentScale_);
        bodySprite_->SetColor({tierColor.x, tierColor.y, tierColor.z});
        bodySprite_->SetAlpha(tierColor.w * currentScale_);
    }

    // 左カッコ
    if (leftParenSprite_) {
        leftParenSprite_->SetIsDraw(true);
        leftParenSprite_->transform_.pos   = pos + layout.leftParenOffset;
        leftParenSprite_->transform_.scale = Vector2(currentScale_, currentScale_);
        leftParenSprite_->SetAlpha(currentScale_);
    }

    // 右カッコ
    if (rightParenSprite_) {
        rightParenSprite_->SetIsDraw(true);
        rightParenSprite_->transform_.pos   = pos + layout.rightParenOffset;
        rightParenSprite_->transform_.scale = Vector2(currentScale_, currentScale_);
        rightParenSprite_->SetAlpha(currentScale_);
    }

    // カッコ内コンボ倍率
    const int32_t intPart    = static_cast<int32_t>(comboBonusValue_);
    const int32_t decPart    = static_cast<int32_t>(std::round((comboBonusValue_ - static_cast<float>(intPart)) * 100.0f));
    const int32_t tenths     = decPart / 10;
    const int32_t hundredths = decPart % 10;

    comboIntDigit_.Update(intPart % 10, pos + layout.comboDigitOffset, scaledCombo, currentScale_);

    if (comboDecimalPointSprite_) {
        comboDecimalPointSprite_->SetIsDraw(true);
        comboDecimalPointSprite_->transform_.pos   = pos + layout.comboDecimalPointOffset;
        comboDecimalPointSprite_->transform_.scale = Vector2(currentScale_, currentScale_);
        comboDecimalPointSprite_->SetAlpha(currentScale_);
    }

    comboDecimalDigits_[0].Update(tenths,     pos + layout.comboDecimalDigitOffset,                         scaledCombo, currentScale_);
    comboDecimalDigits_[1].Update(hundredths, pos + layout.comboDecimalDigitOffset + layout.comboDecimalSpacing, scaledCombo, currentScale_);
}

void KillBonusSimKillUI::SetAllVisible(bool visible) {
    for (auto* s : {simKillLabelSprite_.get(), multiplyIconSprite_.get(), bodySprite_.get(),
                    leftParenSprite_.get(), rightParenSprite_.get(), comboDecimalPointSprite_.get()}) {
        if (s) { s->SetIsDraw(visible); }
    }

    if (!visible) {
        killCountDigits_[0].Hide();
        killCountDigits_[1].Hide();
        comboIntDigit_.Hide();
        comboDecimalDigits_[0].Hide();
        comboDecimalDigits_[1].Hide();
    }
}
