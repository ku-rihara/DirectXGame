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

    // パラメータ
    Vector2 gaugePosition_ = {100.0f, 50.0f};
    Vector2 gaugeScale_    = {1.0f, 1.0f};

public:
    // Getter

    float GetTimerRatio() const { return timerRatio_; }

    void SetPosition(const Vector2& pos) { gaugePosition_ = pos; }
    void SetScale(const Vector2& scale) { gaugeScale_ = scale; }
    void SetTimer(float currentTimer, float maxTimer);
};