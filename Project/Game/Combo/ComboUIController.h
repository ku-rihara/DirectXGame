#pragma once

#include "ComboUI.h"

#include "Editor/ParameterEditor/GlobalParameter.h"
// behavior
#include "Behavior/BaseComboUIBehavior.h"
// std
#include <array>
#include <cstdint>
#include <memory>

/// <summary>
/// コンボUIの制御クラス
/// </summary>
class ComboUIController {
private:
    struct ScalingParameter {
        Vector2 amplitudeScale;
    };

public:
    ComboUIController()  = default;
    ~ComboUIController() = default;

   
    void Init();//<初期化
    void ChangeCountUPAnimation(); //< カウントアップアニメーション変更

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="comboNum">コンボ数</param>
    void Update(const int32_t& comboNum);

    /// <summary>
    /// 時間に応じたアルファ値適応
    /// </summary>
    /// <param name="comboTime">コンボ時間</param>
    /// <param name="comboMaxTime">コンボ最大時間</param>
    void AlphaAdaptForTime(const float& comboTime, const float& comboMaxTime);

    /// <summary>
    /// ビヘイビア変更
    /// </summary>
    /// <param name="behavior">新しいビヘイビア</param>
    void ChangeBehavior(std::unique_ptr<BaseComboUIBehavior> behavior);

    void AdjustParam(); //< パラメータ調整
    void RegisterParams(); //< パラメータバインド

private:
    GlobalParameter* globalParameter_; // グローバルパラメータ
    const std::string groupName_ = "ComboUI"; // グループ名

    // parameter
    ScalingParameter parameter_;

    // Variants
    Vector2 baseScale_;
    std::array<std::unique_ptr<ComboUI>, 3> comboSprites_;
    float alpha_;

    // behavior
    std::unique_ptr<BaseComboUIBehavior> behavior_;

public:
    ///* getter *//
    const ScalingParameter& GetScalingParameter() const { return parameter_; }

    ///* setter *//
    void SetBaseScale(const Vector2& pos) { baseScale_ = pos; }
};