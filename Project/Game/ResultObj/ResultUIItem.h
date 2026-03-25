#pragma once

#include "2d/Sprite.h"
#include "Easing/Easing.h"
#include "Vector2.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

/// <summary>
/// リザルトUIの1項目（ラベル + 数字桁 + 単位）
/// スケールイン → カウントアップ の2段階アニメを行う
/// </summary>
class ResultUIItem {
public:
    struct Config {
        Vector2 basePos          = {640.f, 300.f}; ///< 項目全体の基準座標
        Vector2 labelOffset      = {-200.f, 0.f};  ///< ラベルのオフセット
        Vector2 numberBaseOffset = {60.f,   0.f};  ///< 先頭桁（最大桁）のオフセット
        float   digitSpacing     = 40.f;            ///< 桁間スペーシング
        Vector2 unitOffset       = {220.f,  0.f};  ///< 単位スプライトのオフセット
        Vector2 baseScale        = {1.f,    1.f};  ///< スケールイン後の最終スケール
        Vector2 digitScaleOffset = {0.f,    0.f};  ///< 桁スプライトのスケールオフセット（baseScale に加算）
        float   startDelay       = 0.f;             ///< Start() 後、スケールイン開始までの待機時間（秒）
        int32_t digitCount       = 3;               ///< 桁数（GlobalParameter非登録）
    };

    void Init(const std::string& labelTex, const std::string& unitTex,
              int32_t targetValue, const Config& cfg);

    void SetConfig(const Config& cfg) { config_ = cfg; }

    /// <summary>スケールインアニメーション開始</summary>
    void Start();

    void Update(float dt);

    bool IsFinished() const { return phase_ == Phase::kDone; }

private:
    void ApplyPositions();
    void ApplyScale(const Vector2& scale);
    void UpdateDigits(int32_t value);

    enum class Phase { kIdle, kWait, kScaleIn, kCountUp, kDone };

    std::unique_ptr<KetaEngine::Sprite>              labelSprite_;
    std::vector<std::unique_ptr<KetaEngine::Sprite>> digitSprites_;
    std::unique_ptr<KetaEngine::Sprite>              unitSprite_;

    KetaEngine::Easing<Vector2> scaleEasing_;
    KetaEngine::Easing<float>   countEasing_;

    Vector2 currentScale_ = {};
    float   displayValue_ = 0.f;
    float   targetValue_  = 0.f;
    float   waitTimer_    = 0.f;
    Config  config_       = {};
    Phase   phase_        = Phase::kIdle;
};
