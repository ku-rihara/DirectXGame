#pragma once

#include "KillCountUI.h"
#include "Editor/ParameterEditor/GlobalParameter.h"

// std
#include <array>
#include <cstdint>
#include <memory>

/// <summary>
/// キルカウントUIの制御クラス
/// </summary>
class KillCountUIController {
public:
    KillCountUIController()  = default;
    ~KillCountUIController() = default;

    void Init();

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="killCount">キル数</param>
    void Update(int32_t killCount);

    void AdjustParam();
    void RegisterParams();

private:
    KetaEngine::GlobalParameter* globalParameter_;
    std::string groupName_ = "KillCountUI";

    // parameter
    Vector2 basePosition_;
    Vector2 digitOffset_;
    Vector2 baseScale_;
    float rotateY_ = 0.0f;

    // variants
    std::array<std::unique_ptr<KillCountUI>, 3> killCountSprites_;
    float alpha_ = 1.0f;

public:
    ///* setter *//
    void SetGroupName(const std::string& name) { groupName_ = name; }
    void SetBaseScale(const Vector2& scale) { baseScale_ = scale; }
    void SetBasePosition(const Vector2& pos) { basePosition_ = pos; }
    void SetAlpha(float alpha) { alpha_ = alpha; }
};
