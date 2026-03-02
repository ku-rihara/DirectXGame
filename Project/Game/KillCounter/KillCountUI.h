#pragma once

#include "2d/Sprite.h"
#include "Editor/ParameterEditor/GlobalParameter.h"

// std
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <functional>

/// <summary>
/// キルカウント数値表示UI
/// </summary>
class KillCountUI {
public:
    enum class KillCountDigit {
        ONE,
        TWO,
        THREE,
        Count,
    };

public:
    KillCountUI()  = default;
    ~KillCountUI() = default;

    void Init(const KillCountDigit& digit);
    void Update(const Vector2& scale, float alpha);
    void CalculateNumber(int32_t value);
    void CreateGroupName(const KillCountDigit& digit);

    void AdjustParam();
    void RegisterParams();

private:
    void InitDigitCalculators();

private:
    static constexpr float kUVScaleStep = 0.1f; //< テクスチャ上の1桁分のUV幅 (10数字分割)

    ///* globalParameter *//
    KetaEngine::GlobalParameter* globalParameter_;
    std::string groupName_;

    std::unordered_map<KillCountDigit, std::function<void(int32_t)>> digitCalculators_;

    ///* parameter *//
    KillCountDigit killCountDigit_ = KillCountDigit::ONE;
    Vector2 position_;
    Vector2 positionOffset_ = { 0.0f, 0.0f }; //< 位置オフセット (最終位置 = 基準位置 + オフセット)
    Vector2 scaleOffset_    = { 1.0f, 1.0f }; //< スケールオフセット (最終スケール = 基準スケール * オフセット)
    int32_t valueForDigit_;
    float uvPosX_;
    float rotateY_ = 0.0f;

    //* variables *//
    bool isVisible_;

    ///* Sprite *//
    std::unique_ptr<KetaEngine::Sprite> sprite_;

public:
    ///* Getter *//
    const KillCountDigit& GetKillCountDigit() const { return killCountDigit_; }

    ///* Setter *//
    void SetKillCountDigit(const KillCountDigit& digit) { killCountDigit_ = digit; }
    void SetPosition(const Vector2& pos) { position_ = pos; }
    void SetRotateY(float rotateY) { rotateY_ = rotateY; }
};
