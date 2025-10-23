#pragma once
#include "BaseComboLevelBackObj.h"

/// <summary>
/// スタジアムライトエフェクトクラス
/// </summary>
class StadiumLightEffect : public BaseComboLevelBackObj {
public:
    StadiumLightEffect()           = default;
    ~StadiumLightEffect() override = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="filename">ファイル名</param>
    void Init(const std::string& filename) override;

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="playSpeed">再生速度</param>
    void Update(const float& playSpeed) override;

private:
    bool isPulseOneCycleEnd_ = false;

public:
    const bool& GetIsPulseOneCycleEnd() const { return isPulseOneCycleEnd_; }
    void SetIsPulseOneCycleEnd(const bool& isEnd) { isPulseOneCycleEnd_ = isEnd; }
};