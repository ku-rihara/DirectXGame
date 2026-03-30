#include "KillBonusComboUI.h"
#include "Vector3.h"
#include <cmath>

void KillBonusComboUI::Init() {
    bgSprite_.reset(KetaEngine::Sprite::Create("default.dds", false));
    if (bgSprite_) {
        bgSprite_->SetAnchorPoint({0.5f, 0.5f});
    }

    comboLabelSprite_.reset(KetaEngine::Sprite::Create("UI/KillComboBonus.dds", false));
    if (comboLabelSprite_) {
        comboLabelSprite_->SetAnchorPoint({0.5f, 0.5f});
    }

    comboTextSprite_.reset(KetaEngine::Sprite::Create("UI/ComboTextUI.dds", false));
    if (comboTextSprite_) {
        comboTextSprite_->SetAnchorPoint({0.5f, 0.5f});
    }

    multiplyIconSprite_.reset(KetaEngine::Sprite::Create("UI/MultiplyIcon.dds", false));
    if (multiplyIconSprite_) {
        multiplyIconSprite_->SetAnchorPoint({0.5f, 0.5f});
    }

    comboDecimalPointSprite_.reset(KetaEngine::Sprite::Create("UI/DecimalPoint.dds", false));
    if (comboDecimalPointSprite_) {
        comboDecimalPointSprite_->SetAnchorPoint({0.5f, 0.5f});
    }

    leftParenSprite_.reset(KetaEngine::Sprite::Create("UI/ALeftParenthesis.dds", false));
    if (leftParenSprite_) {
        leftParenSprite_->SetAnchorPoint({0.5f, 0.5f});
    }

    rightParenSprite_.reset(KetaEngine::Sprite::Create("UI/ARightParenthesis.dds", false));
    if (rightParenSprite_) {
        rightParenSprite_->SetAnchorPoint({0.5f, 0.5f});
    }

    comboDigit_.Init("ComboMul");
    comboDecimalDigits_[0].Init("ComboDecimal_1");
    comboDecimalDigits_[1].Init("ComboDecimal_2");
    comboCountDigits_[0].Init("ComboCount_1");
    comboCountDigits_[1].Init("ComboCount_10");
    comboCountDigits_[2].Init("ComboCount_100");

    spawnEasing_.Init("KillBonusSpawn.json");
    spawnEasing_.SetAdaptValue(&currentScale_);

    closeEasing_.Init("KillBonusClose.json");
    closeEasing_.SetAdaptValue(&currentScale_);

    SetAllVisible(false);
    state_ = State::Hidden;
}

void KillBonusComboUI::Spawn(float comboBonusValue, int32_t comboCount) {
    comboBonusValue_ = comboBonusValue;
    comboCount_      = comboCount;
    currentScale_    = 0.0f;
    spawnEasing_.Reset();
    state_ = State::Spawning;
    SetAllVisible(true);
}

void KillBonusComboUI::Close() {
    if (state_ == State::Hidden) {
        return;
    }
    closeEasing_.SetStartValue(currentScale_);
    closeEasing_.Reset();
    state_ = State::Closing;
}

void KillBonusComboUI::Update(float deltaTime, const KillBonusComboUILayout& layout) {
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

    const Vector2 scaledDigit = layout.digitScale * currentScale_;
    const Vector2 scaledCount = layout.comboCountScale * currentScale_;
    const Vector2& pos        = layout.basePos;
    const Vector4 tierColor   = GetKillBonusTierColor(comboCount_, layout.colorTiers);

    // 背景
    if (bgSprite_) {
        bgSprite_->SetIsDraw(true);
        bgSprite_->transform_.pos   = pos + layout.bgOffset;
        bgSprite_->transform_.scale = layout.bgScale * currentScale_;
        bgSprite_->SetColor({layout.bgColor.x, layout.bgColor.y, layout.bgColor.z});
        bgSprite_->SetAlpha(layout.bgColor.w * currentScale_);
    }

    // ラベル
    if (comboLabelSprite_) {
        comboLabelSprite_->SetIsDraw(true);
        comboLabelSprite_->transform_.pos   = pos + layout.labelOffset;
        comboLabelSprite_->transform_.scale = Vector2(currentScale_, currentScale_);
        comboLabelSprite_->SetColor({tierColor.x, tierColor.y, tierColor.z});
        comboLabelSprite_->SetAlpha(tierColor.w * currentScale_);
    }

    // コンボテキスト
    if (comboTextSprite_) {
        comboTextSprite_->SetIsDraw(true);
        comboTextSprite_->transform_.pos   = pos + layout.comboTextOffset;
        comboTextSprite_->transform_.scale = Vector2(currentScale_, currentScale_);
        comboTextSprite_->SetAlpha(currentScale_);
    }

    // MultiplyIcon
    if (multiplyIconSprite_) {
        multiplyIconSprite_->SetIsDraw(true);
        multiplyIconSprite_->transform_.pos   = pos + layout.multiplyIconOffset;
        multiplyIconSprite_->transform_.scale = Vector2(currentScale_, currentScale_);
        multiplyIconSprite_->SetColor({tierColor.x, tierColor.y, tierColor.z});
        multiplyIconSprite_->SetAlpha(tierColor.w * currentScale_);
    }

    // コンボ倍率（整数 + 小数点 + 小数2桁）
    const int32_t intPart    = static_cast<int32_t>(comboBonusValue_);
    const int32_t decPart    = static_cast<int32_t>(std::round((comboBonusValue_ - static_cast<float>(intPart)) * 100.0f));
    const int32_t tenths     = decPart / 10;
    const int32_t hundredths = decPart % 10;

    const Vector3 tierRGB = {tierColor.x, tierColor.y, tierColor.z};

    comboDigit_.SetColor(tierRGB);
    comboDigit_.Update(intPart % 10, pos + layout.digitOffset, scaledDigit, tierColor.w * currentScale_);

    if (comboDecimalPointSprite_) {
        comboDecimalPointSprite_->SetIsDraw(true);
        comboDecimalPointSprite_->transform_.pos   = pos + layout.decimalPointOffset;
        comboDecimalPointSprite_->transform_.scale = Vector2(currentScale_, currentScale_);
        comboDecimalPointSprite_->SetColor(tierRGB);
        comboDecimalPointSprite_->SetAlpha(tierColor.w * currentScale_);
    }

    comboDecimalDigits_[0].SetColor(tierRGB);
    comboDecimalDigits_[1].SetColor(tierRGB);
    comboDecimalDigits_[0].Update(tenths, pos + layout.decimalDigitOffset, scaledDigit, tierColor.w * currentScale_);
    comboDecimalDigits_[1].Update(hundredths, pos + layout.decimalDigitOffset + layout.digitSpacing, scaledDigit, tierColor.w * currentScale_);

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

    // カッコ内コンボ数（1の位・10の位・100の位）
    const int32_t countOnes     = comboCount_ % 10;
    const int32_t countTens     = (comboCount_ / 10) % 10;
    const int32_t countHundreds = (comboCount_ / 100) % 10;

    for (auto& d : comboCountDigits_) { d.SetColor(tierRGB); }
    comboCountDigits_[2].Update(countHundreds, pos + layout.comboCountOffset,                               scaledCount, currentScale_, comboCount_ >= 100 || showMaxDigits_);
    comboCountDigits_[1].Update(countTens,     pos + layout.comboCountOffset + layout.comboCountSpacing,     scaledCount, currentScale_, comboCount_ >= 10  || showMaxDigits_);
    comboCountDigits_[0].Update(countOnes,     pos + layout.comboCountOffset + layout.comboCountSpacing * 2, scaledCount, currentScale_);
}

void KillBonusComboUI::SetAllVisible(bool visible) {
    for (auto* sprite : {bgSprite_.get(), comboLabelSprite_.get(), comboTextSprite_.get(),
             multiplyIconSprite_.get(), comboDecimalPointSprite_.get(),
             leftParenSprite_.get(), rightParenSprite_.get()}) {
        if (sprite) {
            sprite->SetIsDraw(visible);
        }
    }

    if (!visible) {
        comboDigit_.Hide();
        comboDecimalDigits_[0].Hide();
        comboDecimalDigits_[1].Hide();
        comboCountDigits_[0].Hide();
        comboCountDigits_[1].Hide();
        comboCountDigits_[2].Hide();
    }
}
