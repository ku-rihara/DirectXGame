#pragma once
#include "Animation/Object3DAnimation.h"
#include "Audience.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Vector2.h"
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

private:
    const int32_t audienceMaxNum_ = 50;

    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    const std::string groupName_                  = "AudienceController";

    std::array<std::unique_ptr<Audience>, 50> audiences_;

    // 列の設定
    Vector2 baseSeatsZYPos_{0.0f, 0.0f}; 
    Vector2 seatsRowZYOffset_{0.0f, 2.0f};

    // transform
    Vector3 baseTransformScale_;
};