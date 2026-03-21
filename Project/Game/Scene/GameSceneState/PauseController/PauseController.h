#pragma once

/// <summary>
/// ポーズ入力を管理するクラス
/// </summary>
class PauseController {
public:
    PauseController()  = default;
    ~PauseController() = default;

    /// <summary>
    /// ポーズボタンがトリガーされたか（押した瞬間のみ true）
    /// キーボード: P キー / ゲームパッド: Start ボタン
    /// </summary>
    bool IsPauseTriggered() const;
};
