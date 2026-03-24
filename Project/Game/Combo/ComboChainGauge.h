#pragma once

#include "2d/Sprite.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Vector2.h"
#include "Vector3.h"
// std
#include <memory>
#include <string>

/// <summary>
/// コンボ継続可能時間を表示するゲージ
/// </summary>
class ComboChainGauge {
public:
    ComboChainGauge()  = default;
    ~ComboChainGauge() = default;

    void Init();

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="comboTime">残りコンボ時間</param>
    /// <param name="comboMaxTime">コンボ最大時間</param>
    /// <param name="alpha">適用するアルファ値</param>
    void Update(float comboTime, float comboMaxTime, float alpha);

    void AdjustParam();
    void RegisterParams();

private:
    KetaEngine::GlobalParameter* globalParameter_;
    const std::string groupName_ = "ComboChainGauge";

    std::unique_ptr<KetaEngine::Sprite> gaugeSprite_;

    Vector2 position_;
    Vector2 scale_;
    Vector3 color_;
};
