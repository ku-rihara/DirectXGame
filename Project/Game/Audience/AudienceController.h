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

private:
    void AdaptPositions(int32_t index);
    void RegisterParams();

    // enumからstring
    std::string SeatsRowToString(SeatsRow seatsRow);

private:
    const int32_t audienceMaxNum_ = 50;

    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    const std::string groupName_                  = "AudienceController";

    std::array<std::unique_ptr<Audience>, 50> audiences_;
    std::array<Vector2, static_cast<int32_t>(SeatsRow::COUNT)> seatsRowZYPos_;

};