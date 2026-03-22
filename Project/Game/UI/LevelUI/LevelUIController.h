#pragma once

#include "LevelUI.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include <array>
#include <cstdint>
#include <memory>

/// <summary>
/// レベルUIの制御クラス（数字桁 + ラベルスプライト）
/// </summary>
class LevelUIController {
public:
    LevelUIController()  = default;
    ~LevelUIController() = default;

    void Init();
    void Update(int32_t level);

    void AdjustParam();
    void RegisterParams();

private:
    KetaEngine::GlobalParameter* globalParameter_;
    const std::string groupName_ = "LevelUI";

    std::array<std::unique_ptr<LevelUI>, static_cast<int32_t>(LevelUI::Digit::Count)> digits_;
    std::unique_ptr<KetaEngine::Sprite> labelSprite_;

    Vector2 basePosition_;
    Vector2 baseScale_;
    Vector2 digitOffset_;
    Vector2 labelOffset_;
    float   alpha_ = 1.0f;
};
