#pragma once

#include "2d/Sprite.h"
#include "Easing/Easing.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "UI/NumberDigitUI/NumberDigitUI.h"
#include "Vector2.h"
// std
#include <array>
#include <cstdint>
#include <memory>

/// <summary>
/// デスタイマーゲージ
/// </summary>
class DeathTimerGauge {
public:
    // ゲージの状態
    enum class GaugeState {
        Safe,
        Normal,
        Danger,
        Count,
    };

    struct HeatBeat {
        KetaEngine::Easing<Vector2> heatBeatEase;
        std::array<float, static_cast<int32_t>(GaugeState::Count)> heatBeatSpeedRate;
    };

public:
    DeathTimerGauge()  = default;
    ~DeathTimerGauge() = default;

    // 初期化、更新
    void Init();
    void Update(float deltaTime);
   
    void AdjustParam();
    void RegisterParams();

private:
    void UpdateGaugeUV(float deltaTime);
    void UpdateGaugeColor();
    void UpdatePercentDigits();
    void SpriteInit();

private:
    KetaEngine::GlobalParameter* globalParameter_;
    std::string groupName_ = "DeathTimerGauge";

    // スプライト
    std::unique_ptr<KetaEngine::Sprite> frameSprite_      = nullptr;
    std::unique_ptr<KetaEngine::Sprite> gaugeSprite_      = nullptr;
    std::unique_ptr<KetaEngine::Sprite> bombScrollSprite_ = nullptr;
    // アイコン (Safe=Happy, Normal=Normal, Danger=Angry)
    std::array<std::unique_ptr<KetaEngine::Sprite>, static_cast<int32_t>(GaugeState::Count)> gaugeIcons_;
    // 静的スプライト
    std::unique_ptr<KetaEngine::Sprite> nameSprite_      = nullptr;
    std::unique_ptr<KetaEngine::Sprite> iconFrameSprite_ = nullptr;

    // タイマー関連
    float timerRatio_   = 0.0f; // 実際のストレス比率（目標値）
    float displayRatio_ = 0.0f; // 表示用イージング値
    float gaugeEaseSpeed_ = 3.0f; // イージング速度

    // UVスクロール関連
    float uvScrollSpeed_; // スクロール速度
    float uvOffset_;      // 現在のUVオフセット

    //　ハートUIの鼓動
    HeatBeat heatBeat_;

    // 色変更関連
    GaugeState currentState_ = GaugeState::Safe;

    // 各段階の閾値
    float dangerThreshold_;
    float normalThreshold_;

    // 各段階の色
    Vector4 safeColor_;
    Vector4 normalColor_;
    Vector4 dangerColor_;

    // ストレス%数字表示
    static constexpr int32_t kPercentDigitCount = 3;
    std::array<NumberDigitUI, kPercentDigitCount> percentDigits_;
    Vector2 percentBasePos_         = {0.0f, 0.0f};
    Vector2 percentBaseScale_       = {1.0f, 1.0f};
    Vector2 percentDigitOffset_     = {30.0f, 0.0f};
    Vector2 percentSpriteMultiplier_ = {1.0f, 1.0f};

public:
    // Getter
    float GetTimerRatio() const { return timerRatio_; }
    float GetDisplayRatio() const { return displayRatio_; }
    GaugeState GetCurrentState() const { return currentState_; }

    void SetSpriteScales(const Vector2& scale);
    void SetTimer(float currentTimer, float maxTimer);
    void PlayTimerRecoveryScaling();
};