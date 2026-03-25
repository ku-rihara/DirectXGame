#include "ResultUIItem.h"

void ResultUIItem::Init(const std::string& labelTex, const std::string& unitTex,
                        int32_t targetValue, const Config& cfg) {
    config_       = cfg;
    targetValue_  = static_cast<float>(targetValue);
    displayValue_ = 0.f;
    phase_        = Phase::kIdle;

    // ラベルスプライト
    labelSprite_.reset(KetaEngine::Sprite::Create(labelTex));
    labelSprite_->SetIsDraw(true);

    // 数字スプライト（先頭桁から右へ）
    digitSprites_.clear();
    digitSprites_.resize(static_cast<size_t>(cfg.digitCount));
    for (int32_t i = 0; i < cfg.digitCount; ++i) {
        digitSprites_[i].reset(KetaEngine::Sprite::Create("Number/Numbers.dds"));
        digitSprites_[i]->SetUVScale({0.1f, 1.0f});
        digitSprites_[i]->SetUVPosition({0.f, 0.f});
        digitSprites_[i]->SetIsDraw(true);
    }

    // 単位スプライト
    unitSprite_.reset(KetaEngine::Sprite::Create(unitTex));
    unitSprite_->SetIsDraw(true);

    // 初期位置・スケール適用
    ApplyPositions();
    ApplyScale(Vector2::ZeroVector());

    // スケールイージング
    scaleEasing_.Init("ResultItemScaleIn.json");
    scaleEasing_.SetAdaptValue(&currentScale_);
    scaleEasing_.Reset();
    scaleEasing_.SetEndValue(cfg.baseScale);
    scaleEasing_.SetOnFinishCallback([this]() {
        displayValue_ = 0.f;
        countEasing_.Reset();
        phase_ = Phase::kCountUp;
    });

    // カウントアップイージング
    countEasing_.Init("ResultCountUp.json");
    countEasing_.SetAdaptValue(&displayValue_);
    countEasing_.Reset();
    countEasing_.SetEndValue(targetValue_);
    countEasing_.SetOnFinishCallback([this]() {
        UpdateDigits(static_cast<int32_t>(targetValue_));
        phase_ = Phase::kDone;
    });
}

void ResultUIItem::Start() {
    currentScale_ = {0.f, 0.f};
    waitTimer_    = 0.f;
    phase_        = Phase::kWait;
}

void ResultUIItem::Update(float dt) {
    // 位置を毎フレーム Config から適用
    ApplyPositions();

    switch (phase_) {
    case Phase::kIdle:
        ApplyScale(Vector2::ZeroVector());
        break;

    case Phase::kWait:
        ApplyScale(Vector2::ZeroVector());
        waitTimer_ += dt;
        if (waitTimer_ >= config_.startDelay) {
            phase_ = Phase::kScaleIn;
            scaleEasing_.Reset();
        }
        break;

    case Phase::kScaleIn:
        scaleEasing_.Update(dt);
        ApplyScale(currentScale_);
        break;

    case Phase::kCountUp:
        countEasing_.Update(dt);
        ApplyScale(config_.baseScale);
        UpdateDigits(static_cast<int32_t>(displayValue_));
        break;

    case Phase::kDone:
        ApplyScale(config_.baseScale);
        break;
    }
}

void ResultUIItem::ApplyPositions() {
    if (labelSprite_) {
        labelSprite_->transform_.pos = config_.basePos + config_.labelOffset;
    }
    for (int32_t i = 0; i < static_cast<int32_t>(digitSprites_.size()); ++i) {
        Vector2 pos = config_.basePos + config_.numberBaseOffset;
        pos.x += config_.digitSpacing * static_cast<float>(i);
        digitSprites_[i]->transform_.pos = pos;
    }
    if (unitSprite_) {
        unitSprite_->transform_.pos = config_.basePos + config_.unitOffset;
    }
}

void ResultUIItem::ApplyScale(const Vector2& scale) {
    const bool isHidden = (scale.x == 0.f && scale.y == 0.f);
    if (labelSprite_) {
        labelSprite_->transform_.scale = scale;
    }
    for (auto& d : digitSprites_) {
        d->transform_.scale = isHidden ? Vector2::ZeroVector() : scale + config_.digitScaleOffset;
    }
    if (unitSprite_) {
        unitSprite_->transform_.scale = scale;
    }
}

void ResultUIItem::UpdateDigits(int32_t value) {
    int32_t count = static_cast<int32_t>(digitSprites_.size());
    for (int32_t i = 0; i < count; ++i) {
        int32_t power = 1;
        for (int32_t j = 0; j < (count - 1 - i); ++j) {
            power *= 10;
        }
        int32_t digit = (value / power) % 10;
        digitSprites_[i]->SetUVPosition({digit * 0.1f, 0.f});
    }
}
