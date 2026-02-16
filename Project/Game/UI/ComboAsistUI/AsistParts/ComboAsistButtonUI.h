#pragma once
#include "BaseComboAsistUI.h"
#include <cstdint>

/// <summary>
/// コンボアシストのボタンUIクラス
/// </summary>
class ComboAsistButtonUI : public BaseComboAsistUI {
public:
    ComboAsistButtonUI()           = default;
    ~ComboAsistButtonUI() override = default;

public:
    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="gamepadButton">XINPUT_GAMEPAD_* のボタン値</param>
    /// <param name="isUnlocked">開放済みか</param>
    void Init(int32_t gamepadButton, bool isUnlocked);
    void Update() override;

    /// <summary>
    /// 開放状態の更新
    /// </summary>
    void SetUnlocked(bool isUnlocked);

private:
    int32_t gamepadButton_ = 0;
    bool isUnlocked_       = true;
};
