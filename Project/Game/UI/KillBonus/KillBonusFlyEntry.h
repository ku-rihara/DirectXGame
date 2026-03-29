#pragma once
#include "2d/Sprite.h"
#include "Easing/Easing.h"
#include "Editor/RibbonTrailEditor/RibbonTrailPlayer.h"
#include "Vector2.h"
#include "Vector3.h"
#include <functional>
#include <memory>
#include <string>

struct KillBonusFlyParam {
    Vector2     endPos;              // 終点スクリーン座標
    Vector2     controlOffset;       // 第1制御点：始点からのオフセット
    float       controlXOffsetRange; // 第1制御点 X ランダム範囲 (-range ~ +range)
    Vector2     controlOffset2;      // 第2制御点：終点からのオフセット
    float       controlXOffsetRange2;// 第2制御点 X ランダム範囲 (-range ~ +range)
    float       duration;            // 移動時間（秒）
    Vector2     spriteScale;         // スプライトスケール
    std::string spriteTexture;       // テクスチャ名
    Vector3     worldStartPos;       // 始点ワールド座標（トレイル用）
};

/// <summary>
/// 撃破時にスポーンし、ベジエ曲線で終点まで飛ぶスプライト
/// </summary>
class KillBonusFlyEntry {
public:
    KillBonusFlyEntry()  = default;
    ~KillBonusFlyEntry() = default;

    /// param startPos  始点スクリーン座標
    void Init(const Vector2& startPos, const KillBonusFlyParam& param);
    void Update(float deltaTime);

private:
    std::unique_ptr<KetaEngine::Sprite> sprite_;

    Vector2 p0_;  // 始点
    Vector2 p1_;  // 第1制御点
    Vector2 p1b_; // 第2制御点
    Vector2 p2_;  // 終点

    float duration_   = 1.0f;
    bool  isFinished_ = false;

    // t（0→1）をイージングで制御
    KetaEngine::Easing<float> tEasing_;
    float                     easedT_ = 0.0f;

    // トレイル（時空歪み）
    KetaEngine::RibbonTrailPlayer trailPlayer_;
    Vector3                       trailFollowPos_; // トレイルのフォロー位置

    std::function<void()> onReachCallback_;

public:
    bool IsFinished() const { return isFinished_; }
    void SetOnReachCallback(std::function<void()> cb) { onReachCallback_ = std::move(cb); }
};
