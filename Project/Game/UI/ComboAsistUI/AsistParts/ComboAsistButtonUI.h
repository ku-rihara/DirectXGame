#pragma once
#include "BaseComboAsistUI.h"
#include <cstdint>
#include <string>

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
    /// <param name="attackName">対応する攻撃名</param>
    void Init(int32_t gamepadButton, bool isUnlocked, const LayoutParam& layout, const std::string& attackName);

    void Update() override;
    void ApplyLayout() override;

    /// <summary>
    /// 開放状態の更新
    /// </summary>
    void SetUnlocked(bool isUnlocked);

    // 表示/非表示（ロック状態を考慮）
    void SetVisible(bool visible) override;

    /// <summary>
    /// 攻撃名が自分の攻撃名と一致する場合のみプッシュスケーリングを再生する
    /// </summary>
    void TryPlayPushScaling(const std::string& attackName);

public:
    const std::string& GetAttackName() const { return attackName_; }

private:
    int32_t gamepadButton_ = 0;
    bool isUnlocked_       = true;
    std::string attackName_;
};