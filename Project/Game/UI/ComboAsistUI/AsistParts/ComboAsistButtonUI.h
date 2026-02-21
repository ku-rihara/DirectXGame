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
    /// <param name="row">行番号</param>
    /// <param name="col">列番号</param>
    /// <param name="layout">レイアウト情報</param>
    void Init(int32_t gamepadButton, bool isUnlocked, int32_t row, int32_t col, const LayoutParam& layout);

    void Update() override;
    void ApplyLayout() override;

    /// <summary>
    /// 開放状態の更新
    /// </summary>
    void SetUnlocked(bool isUnlocked);

    // 表示/非表示（ロック状態を考慮）
    void SetVisible(bool visible) override;

private:
    int32_t gamepadButton_ = 0;
    bool isUnlocked_       = true;
};