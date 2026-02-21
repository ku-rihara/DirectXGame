#pragma once
#include "BaseSupportSpriteUI.h"

/// <summary>
/// コンボアシストの押すボタンを示すスプライトクラス
/// </summary>
class ComboAsistTriggerButtonUI : public BaseSupportSpriteUI {
public:
    ComboAsistTriggerButtonUI()           = default;
    ~ComboAsistTriggerButtonUI() override = default;

    void Init() override;
    void Update() override;
};
