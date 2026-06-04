#pragma once

// Sprite
#include "2d/Sprite.h"
// Easing
#include "Easing/Easing.h"
// Math
#include "Vector2.h"
// std
#include <cstdint>
#include <functional>
#include <memory>
#include <string>

/// <summary>
/// ランク1枚のスプライトシートで表示するクラス
/// </summary>
class ResultRankSprite {
public:
    // スプライトのパラメータ
    struct Config {
        Vector2 basePos;
        Vector2 baseScale;
        float startDelay;
        int32_t rankCount = 3;
    };

    ResultRankSprite()  = default;
    ~ResultRankSprite() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="rank">ランク</param>
    /// <param name="cfg">設定</param>
    void Init(int32_t rank, const Config& cfg);

    // アニメーション開始、更新、完了判定
    void Start();
    void Update(float dt);
    bool IsFinished() const;

    void SetConfig(const Config& cfg);

private:
    // スプライトのスケールを適用
    void ApplyScale(const Vector2& scale);

    // フェーズごとの更新関数
    void UpdateIdle(float);
    void UpdateWait(float dt);
    void UpdateScaleIn(float dt);
    void UpdateDone(float);

    // アニメーションのフェーズ
    enum class Phase {
        kIdle,
        kWait,
        kScaleIn,
        kDone
    };

private:
    // スプライト、イージングデータ
    std::unique_ptr<KetaEngine::Sprite> sprite_;
    KetaEngine::Easing<Vector2> scaleEasing_;

    // 現在のスケール
    Vector2 currentScale_ = {};
    // 待機タイマー
    float waitTimer_ = 0.f;

    // フェーズごとの更新処理
    std::function<void(float)> phaseUpdate_;

    // 設定データ、アニメーションフェーズ
    Config config_ = {};
    Phase phase_   = Phase::kIdle;

    // テクスチャファイル名
    const std::string textureFileName_ = "ResultUI/RankSprite.dds";
};
