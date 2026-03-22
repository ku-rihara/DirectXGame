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
    void RecordKillCount(int32_t count);
    void RecordLevel(int32_t level);
    void Reset();

    int32_t GetMaxComboCount() const;
    int32_t GetTotalKillCount() const;
    int32_t GetReachedLevel() const;

private:
    GameClearInfo()  = default;
    ~GameClearInfo() = default;

    int32_t maxComboCount_ = 0;
    int32_t killCount_     = 0;
    int32_t reachedLevel_  = 1;
};
