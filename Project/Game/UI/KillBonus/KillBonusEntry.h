#pragma once

#include "UI/NumberDigitUI/NumberDigitUI.h"
#include "2d/Sprite.h"
#include "Easing/Easing.h"
#include "Vector2.h"
#include <array>
#include <cstdint>
#include <memory>

/// レイアウトパラメータ（KillBonusControllerが保持、Updateごとに渡す）
struct KillBonusLayoutParam {
    Vector2 recoveryOffset;
    Vector2 comboOffset;           // コンボ整数部の桁オフセット
    Vector2 comboDecimalPointOffset; // 小数点スプライトのオフセット
    Vector2 comboDecimalDigitOffset; // 小数第1位桁のオフセット（第2位は+digitSpacing）
    Vector2 comboLabelOffset;
    Vector2 simKillOffset;
    Vector2 simKillLabelOffset;
    Vector2 digitSpacing;  // 桁間の間隔
    Vector2 digitScale;
    Vector2 baseScale = {1.0f, 1.0f};  // スプライト全体のベーススケール
    float   displayDuration = 2.0f;    // スポーン後の表示時間
};

/// キルボーナス通知1エントリ
class KillBonusEntry {
public:
    enum class State { Spawning, Displaying, Closing, Finished };

public:
    KillBonusEntry()  = default;
    ~KillBonusEntry() = default;

    // 生成
    void Init(float comboBonusValue, bool hasSimKill, int32_t simKillBonusValue, const Vector2& spawnPos);

    // 更新（毎フレーム呼ぶ）
    void Update(float deltaTime, const KillBonusLayoutParam& layout);

    // 新エントリが来て押し出されるとき、目標位置を再設定
    void ShiftTo(const Vector2& newTarget);

    // シムキル確定後に遡って設定
    void SetSimKill(int32_t bonusValue);

    // 表示数超過時：位置を動かさずその場でクローズ
    void ForceClose();

    bool IsFinished() const { return state_ == State::Finished; }

private:
    std::unique_ptr<KetaEngine::Sprite> recoverySprite_;

    NumberDigitUI                       comboDigit_;
    std::unique_ptr<KetaEngine::Sprite> comboLabelSprite_;

    std::array<NumberDigitUI, 2>        simKillDigits_;   // [0]=1の位, [1]=10の位
    std::unique_ptr<KetaEngine::Sprite> simKillLabelSprite_;

    bool    hasSimKill_        = false;
    float   comboBonusValue_   = 1.0f;
    int32_t simKillBonusValue_ = 0;

    // ステート
    State state_        = State::Spawning;
    float displayTimer_ = 0.0f;

    // スケールアニメ（spawn: 0→1、close: 1→0）
    float currentScale_ = 0.0f;
    KetaEngine::Easing<float> spawnEasing_;
    KetaEngine::Easing<float> closeEasing_;

    Vector2 currentPos_;
    bool posEasingActive_ = false;
    KetaEngine::Easing<Vector2> posEasing_;

    // コンボボーナス小数表示用
    std::unique_ptr<KetaEngine::Sprite> comboDecimalPointSprite_;
    std::array<NumberDigitUI, 2>        comboDecimalDigits_;  // [0]=十分位, [1]=百分位
};
