#pragma once

// Audience
#include "Audience.h"
// Parameter
#include "Editor/ParameterEditor/GlobalParameter.h"
// math
#include "Vector2.h"
#include "Vector3.h"
// std
#include <array>
#include <cstdint>
#include <memory>
#include <string>

/// <summary>
/// 観客管理
/// </summary>
class AudienceController {
public:
    AudienceController()  = default;
    ~AudienceController() = default;

    // 初期化、更新
    void Init();
    void Update();

    // parameter
    void AdjustParam();

    // appear
    void AppearAudienceByLevel(int32_t level);
    void DisAppearAudience(int32_t level);

private:
    void AdaptPositions(int32_t index);
    void RegisterParams();

    // 列番号から位置を計算
    Vector2 GetSeatsRowZYPos(int32_t row) const;

    // 毎フレーム1体ずつ生成
    void DeferredCreate();

    // DisAppear を毎フレーム数体ずつ処理
    void ProcessPendingDisappear();

private:
    // 観客の最大数
    const int32_t audienceMaxNum_ = 50;

    // global parameter
    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    const std::string groupName_                  = "AudienceController";

    // 観客の配列
    std::array<std::unique_ptr<Audience>, 50> audiences_;

    // 分散生成用カウンタ
    int32_t createdCount_ = 0;

    // DisAppear 段階処理用
    int32_t pendingDisappearLevel_ = -1;
    int32_t pendingDisappearIndex_ = 0;
    static constexpr int32_t kDisappearPerFrame_ = 3;

    // 列の設定
    Vector2 baseSeatsZYPos_   = {0.0f, 0.0f};
    Vector2 seatsRowZYOffset_ = {0.0f, 2.0f};

    float cheerAnimationTimer_;

    // transform
    Vector3 baseTransformScale_;
};