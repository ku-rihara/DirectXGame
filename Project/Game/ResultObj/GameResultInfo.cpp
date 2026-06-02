#include "GameResultInfo.h"

GameResultInfo* GameResultInfo::GetInstance() {
    static GameResultInfo instance;
    return &instance;
}

void GameResultInfo::RecordCombo(int32_t count) {

    // 現在の最大コンボ数を更新
    if (count > maxComboCount_) {
        maxComboCount_ = count;
    }
}

void GameResultInfo::RecordKillCount(int32_t count) {
    // キル数を記録
    killCount_ = count;
}

void GameResultInfo::RecordSurvivalTime(int32_t seconds) {
    // 生存時間を記録
    survivalTimeSec_ = seconds;
}

void GameResultInfo::Reset() {

    // クリア情報をリセット
    maxComboCount_   = 0;
    killCount_       = 0;
    survivalTimeSec_ = 0;
}

int32_t GameResultInfo::GetRank(int32_t scoreForA, int32_t scoreForS) const {

    // score = killCount + survivalTimeSec で判定
    int32_t score = killCount_ + survivalTimeSec_;

    // S
    if (score >= scoreForS) {
        return 2;
    }

    // A
    if (score >= scoreForA) {
        return 1;
    }

    // B
    return 0;
}
