#include "KillBonusEntry.h"
#include <cmath>

void KillBonusEntry::Init(float comboBonusValue, bool hasSimKill, int32_t simKillBonusValue, const Vector2& spawnPos) {
    comboBonusValue_   = comboBonusValue;
    hasSimKill_        = hasSimKill;
    simKillBonusValue_ = simKillBonusValue;
    currentPos_       = spawnPos;
    posEasingActive_  = false;
    state_            = State::Spawning;
    displayTimer_     = 0.0f;

    bgSprite_.reset(KetaEngine::Sprite::Create("UI/KillBonusBG.dds",false));
    if (bgSprite_) {
        bgSprite_->SetLayerNum(-1);
        bgSprite_->SetAnchorPoint({0.5f, 0.5f});
    }

    recoverySprite_.reset(KetaEngine::Sprite::Create("UI/KillRecovery.dds", false));
    if (recoverySprite_) {
        recoverySprite_->SetAnchorPoint({0.5f, 0.5f});
    }

    // コンボボーナス：整数桁
    comboDigit_.Init("ComboMul");

    // コンボボーナス：小数点スプライト
    comboDecimalPointSprite_.reset(KetaEngine::Sprite::Create("UI/DecimalPoint.dds", false));
    if (comboDecimalPointSprite_) {
        comboDecimalPointSprite_->SetAnchorPoint({0.5f, 0.5f});
    }

    // コンボボーナス：小数桁（十分位・百分位）
    comboDecimalDigits_[0].Init("ComboDecimal_1");
    comboDecimalDigits_[1].Init("ComboDecimal_2");

    // コンボボーナスラベル（"コンボボーナスx"）
    comboLabelSprite_.reset(KetaEngine::Sprite::Create("UI/KillComboBonus.dds", false));
    if (comboLabelSprite_) {
        comboLabelSprite_->SetAnchorPoint({0.5f, 0.5f});
    }

    simKillDigits_[0].Init("SimKill_1");
    simKillDigits_[1].Init("SimKill_10");

    // 同時撃破ボーナスラベル（"同時撃破ボーナス"）
    simKillLabelSprite_.reset(KetaEngine::Sprite::Create("UI/SimKillBonus.dds", false));
    if (simKillLabelSprite_) {
        simKillLabelSprite_->SetAnchorPoint({0.5f, 0.5f});
        simKillLabelSprite_->SetIsDraw(hasSimKill_);
    }

    posEasing_.Init("KillBonusPosShift.json");
    posEasing_.SetAdaptValue(&currentPos_);

    currentScale_ = 0.0f;
    spawnEasing_.Init("KillBonusSpawn.json");
    spawnEasing_.SetAdaptValue(&currentScale_);
    spawnEasing_.Reset();

    closeEasing_.Init("KillBonusClose.json");
    closeEasing_.SetAdaptValue(&currentScale_);
}

void KillBonusEntry::Update(float deltaTime, const KillBonusLayoutParam& layout) {
    // ステートマシン
    switch (state_) {
    case State::Spawning:
        spawnEasing_.Update(deltaTime);
        if (posEasingActive_) {
            posEasing_.Update(deltaTime);
            if (posEasing_.IsFinished()) {
                posEasingActive_ = false;
            }
        }
        if (spawnEasing_.IsFinished()) {
            currentScale_ = 1.0f;
            state_        = State::Displaying;
            displayTimer_ = 0.0f;
        }
        break;

    case State::Displaying:
        if (posEasingActive_) {
            posEasing_.Update(deltaTime);
            if (posEasing_.IsFinished()) {
                posEasingActive_ = false;
            }
        }
        displayTimer_ += deltaTime;
        if (displayTimer_ >= layout.displayDuration) {
            state_ = State::Closing;
            closeEasing_.Reset();
        }
        break;

    case State::Closing:
        closeEasing_.Update(deltaTime);
        if (posEasingActive_) {
            posEasing_.Update(deltaTime);
            if (posEasing_.IsFinished()) {
                posEasingActive_ = false;
            }
        }
        if (closeEasing_.IsFinished()) {
            currentScale_ = 0.0f;
            state_        = State::Finished;
        }
        break;

    case State::Finished:
        break;
    }

    // スプライト更新
    const Vector2 spriteScale = layout.baseScale * currentScale_;
    const Vector2 scaledDigit = layout.digitScale * currentScale_;

    // 背景スプライト
    if (bgSprite_) {
        bgSprite_->SetIsDraw(true);
        bgSprite_->transform_.pos   = currentPos_ + layout.bgOffset;
        bgSprite_->transform_.scale = spriteScale * layout.bgScale;
        bgSprite_->SetColor({layout.bgColor.x, layout.bgColor.y, layout.bgColor.z});
        bgSprite_->SetAlpha(layout.bgColor.w * currentScale_);
    }

    // 回復スプライト
    if (recoverySprite_) {
        recoverySprite_->SetIsDraw(true);
        recoverySprite_->transform_.pos   = currentPos_ + layout.recoveryOffset;
        recoverySprite_->transform_.scale = spriteScale;
        recoverySprite_->SetAlpha(1.0f);
    }

    // コンボボーナス値の分解（整数部 + 小数2桁）
    const int32_t intPart  = static_cast<int32_t>(comboBonusValue_);
    const int32_t decPart  = static_cast<int32_t>(std::round((comboBonusValue_ - static_cast<float>(intPart)) * 100.0f));
    const int32_t tenths   = decPart / 10;
    const int32_t hundredths = decPart % 10;

    // 整数桁
    comboDigit_.Update(intPart % 10, currentPos_ + layout.comboOffset, scaledDigit, 1.0f);

    // 小数点スプライト
    if (comboDecimalPointSprite_) {
        comboDecimalPointSprite_->SetIsDraw(true);
        comboDecimalPointSprite_->transform_.pos   = currentPos_ + layout.comboDecimalPointOffset;
        comboDecimalPointSprite_->transform_.scale = spriteScale;
        comboDecimalPointSprite_->SetAlpha(1.0f);
    }

    // 小数桁（十分位・百分位）
    comboDecimalDigits_[0].Update(tenths,    currentPos_ + layout.comboDecimalDigitOffset,                      scaledDigit, 1.0f);
    comboDecimalDigits_[1].Update(hundredths, currentPos_ + layout.comboDecimalDigitOffset + layout.digitSpacing, scaledDigit, 1.0f);

    // コンボボーナスラベル
    if (comboLabelSprite_) {
        comboLabelSprite_->SetIsDraw(true);
        comboLabelSprite_->transform_.pos   = currentPos_ + layout.comboLabelOffset;
        comboLabelSprite_->transform_.scale = spriteScale;
        comboLabelSprite_->SetAlpha(1.0f);
    }

    // 同時撃破ボーナス
    const bool showSim   = hasSimKill_;
    int32_t ones = simKillBonusValue_ % 10;
    int32_t tens = (simKillBonusValue_ / 10) % 10;

    simKillDigits_[0].Update(ones, currentPos_ + layout.simKillOffset,                      scaledDigit, 1.0f, showSim);
    simKillDigits_[1].Update(tens, currentPos_ + layout.simKillOffset + layout.digitSpacing, scaledDigit, 1.0f, showSim && (simKillBonusValue_ >= 10));

    if (simKillLabelSprite_) {
        simKillLabelSprite_->transform_.pos   = currentPos_ + layout.simKillLabelOffset;
        simKillLabelSprite_->transform_.scale = spriteScale;
        simKillLabelSprite_->SetAlpha(showSim ? 1.0f : 0.0f);
        simKillLabelSprite_->SetIsDraw(showSim);
    }
}

void KillBonusEntry::ShiftTo(const Vector2& newTarget) {
    posEasing_.SetStartValue(currentPos_);
    posEasing_.SetEndValue(newTarget);
    posEasing_.Reset();
    posEasingActive_ = true;
}

void KillBonusEntry::ForceClose() {
    if (state_ == State::Finished) { 
        return;
    }
    posEasingActive_ = false;  
    state_           = State::Closing;
    closeEasing_.SetStartValue(currentScale_);
    closeEasing_.Reset();
}

void KillBonusEntry::SetSimKill(int32_t bonusValue) {
    hasSimKill_        = true;
    simKillBonusValue_ = bonusValue;
    if (simKillLabelSprite_) {
        simKillLabelSprite_->SetIsDraw(true);
    }
}
