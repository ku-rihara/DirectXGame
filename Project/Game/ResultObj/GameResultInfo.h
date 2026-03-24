#pragma once

#include <cstdint>

/// <summary>
/// ゲームクリア情報クラス
/// シーンをまたいでゲームクリア時のデータを保持する
/// </summary>
class GameResultInfo {
public:
    static GameResultInfo* GetInstance();

    void RecordCombo(int32_t count);
    void RecordKillCount(int32_t count);
    void RecordLevel(int32_t level);
    void Reset();

private:
    GameResultInfo()  = default;
    ~GameResultInfo() = default;

    int32_t maxComboCount_ = 0;
    int32_t killCount_     = 0;
    int32_t reachedLevel_  = 1;

public:
    int32_t GetMaxComboCount() const;
    int32_t GetTotalKillCount() const;
    int32_t GetReachedLevel() const;
};
