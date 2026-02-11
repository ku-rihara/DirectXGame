#pragma once
#include "BaseComboLevelBackObj.h"

/// <summary>
/// スピーカーエフェクトクラス
/// </summary>
class SpeakerEffect : public BaseComboLevelBackObj {
public:
    SpeakerEffect()           = default;
    ~SpeakerEffect() override = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="filename">ファイル名</param>
    void Init(const std::string& filename) override;

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="playSpeed">再生速度</param>
    void Update(float playSpeed) override;

public:
};