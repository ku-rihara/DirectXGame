#pragma once

#include "2d/Sprite.h"
#include "Easing/Easing.h"
#include "UI/KillBonus/KillBonusColorTier.h"
#include "UI/NumberDigitUI/NumberDigitUI.h"
#include "Vector2.h"
#include "Vector4.h"
#include <array>
#include <cstdint>
#include <memory>

struct KillBonusComboUILayout {
    Vector2 basePos;

    // 背景
    Vector2 bgOffset;
    Vector2 bgScale;
    Vector4 bgColor;

    // ラベル
    Vector2 labelOffset;

    // コンボテキスト
    Vector2 comboTextOffset;

    // MultiplyIcon
    Vector2 multiplyIconOffset;

    // コンボ倍率（整数 + 小数点 + 小数2桁）
    Vector2 digitOffset;
    Vector2 decimalPointOffset;
    Vector2 decimalDigitOffset;
    Vector2 digitSpacing;
    Vector2 digitScale;

    // カッコ
    Vector2 leftParenOffset;
    Vector2 rightParenOffset;

    // カッコ内コンボ数
    Vector2 comboCountOffset;
    Vector2 comboCountSpacing;
    Vector2 comboCountScale;

    // 段階カラー
    std::array<KillBonusColorTier, 3> colorTiers;
};

class KillBonusComboUI {
public:
    enum class State { 
        Hidden,
        Spawning,
        Displaying,
        Closing 
    };

    void Init();
    void Update(float deltaTime, const KillBonusComboUILayout& layout);
    void Spawn(float comboBonusValue, int32_t comboCount);
    void Close();

private:
    void SetAllVisible(bool visible);

private:
    // 背景
    std::unique_ptr<KetaEngine::Sprite> bgSprite_;

    // ラベル
    std::unique_ptr<KetaEngine::Sprite> comboLabelSprite_;

    // コンボテキスト
    std::unique_ptr<KetaEngine::Sprite> comboTextSprite_;

    // MultiplyIcon
    std::unique_ptr<KetaEngine::Sprite> multiplyIconSprite_;

    // コンボ倍率
    NumberDigitUI comboDigit_;
    std::unique_ptr<KetaEngine::Sprite> comboDecimalPointSprite_;
    std::array<NumberDigitUI, 2> comboDecimalDigits_;

    // カッコ
    std::unique_ptr<KetaEngine::Sprite> leftParenSprite_;
    std::unique_ptr<KetaEngine::Sprite> rightParenSprite_;

    // カッコ内コンボ数（3桁）
    std::array<NumberDigitUI, 3> comboCountDigits_;

    float comboBonusValue_;
    int32_t comboCount_;
    bool showMaxDigits_;
    State state_;
    float currentScale_;

    KetaEngine::Easing<float> spawnEasing_;
    KetaEngine::Easing<float> closeEasing_;

public:
    void SetShowMaxDigits(bool show) { showMaxDigits_ = show; }
    bool IsHidden() const { return state_ == State::Hidden; }
};
