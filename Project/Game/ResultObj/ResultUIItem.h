#pragma once

#include "2D/Sprite.h"
#include "Easing/Easing.h"
#include "Vector2.h"
#include <array>
#include <cstdint>
#include <memory>
#include <string>

/// <summary>
/// リザルトUIの1項目
/// </summary>
class ResultUIItem {
public:
    struct Config {
        Vector2 basePos;
        Vector2 labelOffset;
        Vector2 numberBaseOffset;
        float digitSpacing;
        Vector2 unitOffset;
        Vector2 baseScale;
        Vector2 digitScaleOffset;
        float startDelay;
        int32_t digitCount = 3; //< 桁数
    };

    ResultUIItem()  = default;
    ~ResultUIItem() = default;

    void Init(const std::string& labelTex, const std::string& unitTex,
        int32_t targetValue, const Config& cfg);

    void SetConfig(const Config& cfg) { config_ = cfg; }

    // スケールインアニメーション開始
    void Start();

    void Update(float dt);

    bool IsFinished() const { return phase_ == Phase::kDone; }

private:
    void ApplyPositions();
    void ApplyScale(const Vector2& scale);
    void UpdateDigits(int32_t value);

    enum class Phase { 
        kIdle,
        kWait,
        kScaleIn,
        kCountUp,
        kDone };

    static constexpr int32_t kMaxDigits = 3;

    std::unique_ptr<KetaEngine::Sprite>              labelSprite_;
    std::array<std::unique_ptr<KetaEngine::Sprite>, kMaxDigits> digitSprites_;
    std::unique_ptr<KetaEngine::Sprite>              unitSprite_;

    KetaEngine::Easing<Vector2> scaleEasing_;
    KetaEngine::Easing<float> countEasing_;

    Vector2 currentScale_ = {};
    float displayValue_   = 0.f;
    float targetValue_    = 0.f;
    float waitTimer_      = 0.f;
    Config config_        = {};
    Phase phase_          = Phase::kIdle;
};
