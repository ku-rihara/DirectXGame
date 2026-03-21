#pragma once

#include <cstdint>

/// <summary>
/// ゲームクリア情報クラス
/// シーンをまたいでゲームクリア時のデータを保持する
/// </summary>
class GameClearInfo {
public:
    static GameClearInfo* GetInstance();

    void RecordCombo(int32_t count);
    void Reset();
    int32_t GetMaxComboCount() const;

private:
    GameClearInfo()  = default;
    ~GameClearInfo() = default;

    int32_t maxComboCount_ = 0;
};
