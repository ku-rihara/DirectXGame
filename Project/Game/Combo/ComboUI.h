#pragma once

#include "2d/Sprite.h"
#include "Editor/ParameterEditor/GlobalParameter.h"

// std
#include <cstdint>
#include <memory>

/// <summary>
/// コンボ数値表示UI
/// </summary>
class ComboUI {
public:
    enum class ComboDigit {
        ONE,
        TWO,
        THREE,
        Count,
    };

public:
    ComboUI()  = default;
    ~ComboUI() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="digit">桁</param>
    void Init(const ComboDigit& digit);

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="scale">スケール</param>
    /// <param name="alpha">アルファ値</param>
    void Update(const Vector2& scale, const float& alpha);

    /// <summary>
    /// 数値の計算
    /// </summary>
    /// <param name="value">値</param>
    void CalculateNumber(const int32_t& value);

    /// <summary>
    /// グループ名の作成
    /// </summary>
    /// <param name="digit">桁</param>
    void CreateGroupName(const ComboDigit& digit);

    void AdjustParam(); //< パラメータ調整
    void RegisterParams(); //< パラメータバインド

private:
    ///* globalParameter *//
    GlobalParameter* globalParameter_;
    std::string groupName_;

    ///* parameter *//
    ComboDigit comboDigit_ = ComboDigit::ONE;
    Vector2 position_;
    int32_t valueForDigit_;
    float uvPosX_;
    float uvScaleOffset_;

    //* variables *//
    bool isVisible_;

    ///* Sprite *//
    std::unique_ptr<Sprite> sprite_;

public:
    ///* Getter *//
    const ComboDigit& GetComboDigit() const { return comboDigit_; }

    ///* Setter *//
    void SetComboDigit(const ComboDigit& digit) { comboDigit_ = digit; }
};