#pragma once
#include "BaseComboAsistUI.h"
#include <cstdint>

/// <summary>
/// コンボアシストでの矢印UIクラス
/// ボタン間の接続を表す矢印を表示
/// </summary>
class ComboAsistArrowUI : public BaseComboAsistUI {
public:
    ComboAsistArrowUI()           = default;
    ~ComboAsistArrowUI() override = default;

public:
  
    void Init();
    void Update() override;
};
