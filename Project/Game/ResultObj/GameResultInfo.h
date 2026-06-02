#pragma once

#include <cstdint>

/// <summary>
/// ゲームクリア情報クラス
/// シーンをまたいでゲームクリア時のデータを保持する
/// </summary>
class GameResultInfo {
public:
    static GameResultInfo* GetInstance();

    /// <summary>
    // コンボ数を記録し、現在の最大コンボ数を更新する
    /// </summary>
    /// <param name="count"> コンボ数 </param>
    void RecordCombo(int32_t count);

    /// <summary>
    /// キル数を記録する
    /// </summary>
    /// <param name="count"> キル数 </param>
    void RecordKillCount(int32_t count);

    /// <summary>
    /// 生存時間を記録する
    /// </summary>
    /// <param name="seconds"> 生存時間（秒）</param> 
    void RecordSurvivalTime(int32_t seconds);

    // 記録をリセットする
    void Reset();

private:
    GameResultInfo()  = default;
    ~GameResultInfo() = default;

    // ゲームクリア時の情報
    int32_t maxComboCount_   = 0;
    int32_t killCount_       = 0;
    int32_t survivalTimeSec_ = 0;

public:

    // Getter
    int32_t GetMaxComboCount()   const { return maxComboCount_; }
    int32_t GetTotalKillCount()  const { return killCount_; }
    int32_t GetSurvivalTimeSec() const { return survivalTimeSec_; }

    /// <summary>
    /// ランクを返す (B=0, A=1, S=2)
    /// score = killCount + survivalTimeSec で判定
    /// </summary>
    int32_t GetRank(int32_t scoreForA, int32_t scoreForS) const;
};
