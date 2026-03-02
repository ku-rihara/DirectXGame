#pragma once

#include "2d/Sprite.h"
#include "Editor/ParameterEditor/GlobalParameter.h"

// std
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <functional>

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
    void Update(const Vector2& scale, float alpha, const Vector3& color);

    /// <summary>
    /// 数値の計算
    /// </summary>
    /// <param name="value">値</param>
    void CalculateNumber(int32_t value);

    /// <summary>
    /// グループ名の作成
    /// </summary>
    /// <param name="digit">桁</param>
    void CreateGroupName(const ComboDigit& digit);

    void AdjustParam(); //< パラメータ調整
    void RegisterParams(); //< パラメータバインド

private:
    void InitDigitCalculators();
private:

    static constexpr float kUVScaleStep = 0.1f; //< テクスチャ上の1桁分のUV幅

    ///* globalParameter *//
    KetaEngine::GlobalParameter* globalParameter_;
    std::string groupName_;

    std::unordered_map<ComboDigit, std::function<void(int32_t)>> digitCalculators_;

    ///* parameter *//
    ComboDigit comboDigit_ = ComboDigit::ONE;
    Vector2 position_;
    Vector2 positionOffset_ = { 0.0f, 0.0f }; //< 位置オフセット
    Vector2 scaleOffset_    = { 1.0f, 1.0f }; //< スケールオフセット 
    int32_t valueForDigit_;
    float uvPosX_;
    float uvScaleOffset_;
    float rotateY_ = 0.0f;

    //* variables *//
    bool isVisible_;

    ///* Sprite *//
    std::unique_ptr<KetaEngine::Sprite> sprite_;

public:
    ///* Getter *//
    const ComboDigit& GetComboDigit() const { return comboDigit_; }

    ///* Setter *//
    void SetComboDigit(const ComboDigit& digit) { comboDigit_ = digit; }
    void SetPosition(const Vector2& pos) { position_ = pos; }
    void SetRotateY(float rotateY) { rotateY_ = rotateY; }
};