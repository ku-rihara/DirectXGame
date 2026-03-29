#pragma once

#include "UI/NumberDigitUI/NumberDigitUI.h"
#include "UI/KillBonus/KillBonusColorTier.h"
#include "2d/Sprite.h"
#include "Easing/Easing.h"
#include "Vector2.h"
#include "Vector4.h"
#include <array>
#include <cstdint>
#include <memory>

struct KillBonusSimKillUILayout {
    Vector2 basePos;

    // 同時キルボーナスラベル
    Vector2 labelOffset;

    // キル数（2桁）
    Vector2 digitOffset;
    Vector2 digitSpacing;
    Vector2 digitScale     = {1.0f, 1.0f};

    // 体スプライト
    Vector2 bodyOffset;

    // カッコ
    Vector2 leftParenOffset;
    Vector2 rightParenOffset;

    // カッコ内コンボ倍率
    Vector2 comboDigitOffset;
    Vector2 comboDecimalPointOffset;
    Vector2 comboDecimalDigitOffset;
    Vector2 comboDecimalSpacing;
    Vector2 comboDigitScale = {1.0f, 1.0f};

    // MultiplyIcon
    Vector2 multiplyIconOffset;

    // 段階カラー（キル数によって変化）
    std::array<KillBonusSimKillColorTier, 3> colorTiers = {{
        { 2, {1.0f, 1.0f, 1.0f, 1.0f}},
        { 5, {1.0f, 1.0f, 0.0f, 1.0f}},
        {10, {1.0f, 0.3f, 0.0f, 1.0f}},
    }};
};

class KillBonusSimKillUI {
public:
    enum class State { Hidden, Spawning, Displaying, Closing };

    void Init();
    void Update(float deltaTime, const KillBonusSimKillUILayout& layout);
    void Spawn(int32_t killCount, float comboBonusValue);
    void Close();

    void SetShowMaxDigits(bool show) { showMaxDigits_ = show; }
    bool IsHidden() const { return state_ == State::Hidden; }

private:
    void SetAllVisible(bool visible);

    // 同時キルボーナスラベル
    std::unique_ptr<KetaEngine::Sprite> simKillLabelSprite_;

    // キル数
    std::array<NumberDigitUI, 2>        killCountDigits_;  // [0]=1の位, [1]=10の位

    // 体スプライト
    std::unique_ptr<KetaEngine::Sprite> bodySprite_;

    // カッコ
    std::unique_ptr<KetaEngine::Sprite> leftParenSprite_;
    std::unique_ptr<KetaEngine::Sprite> rightParenSprite_;

    // MultiplyIcon
    std::unique_ptr<KetaEngine::Sprite> multiplyIconSprite_;

    // カッコ内コンボ倍率
    NumberDigitUI                       comboIntDigit_;
    std::unique_ptr<KetaEngine::Sprite> comboDecimalPointSprite_;
    std::array<NumberDigitUI, 2>        comboDecimalDigits_;

    int32_t killCount_       = 0;
    float   comboBonusValue_ = 1.0f;
    bool    showMaxDigits_   = false;

    State state_        = State::Hidden;
    float currentScale_ = 0.0f;

    KetaEngine::Easing<float> spawnEasing_;
    KetaEngine::Easing<float> closeEasing_;
};
