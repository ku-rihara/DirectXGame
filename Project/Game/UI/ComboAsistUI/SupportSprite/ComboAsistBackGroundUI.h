#pragma once
#include "BaseSupportSpriteUI.h"

/// <summary>
/// コンボアシストのバックグラウンドスプライトクラス
/// </summary>
class ComboAsistBackGroundUI : public BaseSupportSpriteUI {
public:
    ComboAsistBackGroundUI()           = default;
    ~ComboAsistBackGroundUI() override = default;

    void Init() override;
    void Update() override;
};
