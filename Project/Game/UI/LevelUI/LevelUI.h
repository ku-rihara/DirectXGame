#pragma once

#include "2d/Sprite.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <unordered_map>

/// <summary>
/// レベル数値表示UI（1桁）
/// </summary>
class LevelUI {
public:
    enum class Digit {
        ONE,
        TWO,
        Count,
    };

public:
    LevelUI()  = default;
    ~LevelUI() = default;

    void Init(Digit digit);
    void Update(const Vector2& scale, float alpha);
    void CalculateNumber(int32_t value);
    void SetPosition(const Vector2& pos) { position_ = pos; }

    void AdjustParam();
    void RegisterParams();

private:
    void CreateGroupName(Digit digit);
    void InitDigitCalculators();

private:
    static constexpr float kUVStep = 0.1f;

    KetaEngine::GlobalParameter* globalParameter_;
    std::string groupName_;

    std::unordered_map<Digit, std::function<void(int32_t)>> digitCalculators_;

    Digit   digit_          = Digit::ONE;
    Vector2 position_;
    Vector2 positionOffset_ = {0.0f, 0.0f};
    Vector2 scaleOffset_    = {1.0f, 1.0f};
    int32_t valueForDigit_  = 0;
    float   uvPosX_         = 0.0f;
    bool    isVisible_      = true;

    std::unique_ptr<KetaEngine::Sprite> sprite_;
};
