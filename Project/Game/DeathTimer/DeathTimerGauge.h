#pragma once

#include "2d/Sprite.h"
#include "Combo/ComboUIController.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
// std
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
        Danger  
    };

public:
    DeathTimerGauge()  = default;
    ~DeathTimerGauge() = default;

    // 初期化、更新
    void Init();
    void Update(float deltaTime);
    void Draw();

    void AdjustParam();
    void RegisterParams();

private:
    void UpdateGaugeUV(float deltaTime);
    void UpdateGaugeColor();

private:
    KetaEngine::GlobalParameter* globalParameter_;
    std::string groupName_ = "DeathTimerGauge";

    // スプライト
    std::unique_ptr<KetaEngine::Sprite> frameSprite_      = nullptr;
    std::unique_ptr<KetaEngine::Sprite> gaugeSprite_      = nullptr;
    std::unique_ptr<KetaEngine::Sprite> gaugeIcon_        = nullptr;
    std::unique_ptr<KetaEngine::Sprite> bombScrollSprite_ = nullptr;

    // タイマー関連
    float timerRatio_ = 1.0f;

    // UVスクロール関連
    float uvScrollSpeed_; // スクロール速度
    float uvOffset_; // 現在のUVオフセット

    // 色変更関連
    GaugeState currentState_ = GaugeState::Safe;

    // 各段階の閾値
    float dangerThreshold_ ; 
    float normalThreshold_ ; 

    // 各段階の色
    Vector4 safeColor_;
    Vector4 normalColor_;
    Vector4 dangerColor_;

public:
    // Getter
    float GetTimerRatio() const { return timerRatio_; }
    GaugeState GetCurrentState() const { return currentState_; }

    void SetSpriteScales(const Vector2& scale);
    void SetTimer(float currentTimer, float maxTimer);
};