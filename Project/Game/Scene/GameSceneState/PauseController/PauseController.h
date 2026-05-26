#pragma once

/// <summary>
/// ポーズ入力を管理するクラス
/// </summary>
class PauseController {
public:
    PauseController()  = default;
    ~PauseController() = default;

    /// <summary>
    /// ポーズボタンがトリガーされたか
    /// キーボード: P キー / ゲームパッド: Start ボタン
    /// </summary>
    bool IsPauseTriggered() const;
};
