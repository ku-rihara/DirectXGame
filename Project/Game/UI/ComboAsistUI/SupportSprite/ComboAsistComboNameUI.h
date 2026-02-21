#pragma once
#include "BaseSupportSpriteUI.h"

/// <summary>
/// コンボアシストで表示中のコンボ名スプライトクラス
/// </summary>
class ComboAsistComboNameUI : public BaseSupportSpriteUI {
public:
    ComboAsistComboNameUI()           = default;
    ~ComboAsistComboNameUI() override = default;

    void Init() override;
    void Update() override;
};
