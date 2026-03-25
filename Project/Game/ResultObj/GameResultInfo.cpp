#include "GameResultInfo.h"

GameResultInfo* GameResultInfo::GetInstance() {
    static GameResultInfo instance;
    return &instance;
}

void GameResultInfo::RecordCombo(int32_t count) {
    if (count > maxComboCount_) {
        maxComboCount_ = count;
    }
}

void GameResultInfo::RecordKillCount(int32_t count) {
    killCount_ = count;
}

void GameResultInfo::RecordLevel(int32_t level) {
    if (level > reachedLevel_) {
        reachedLevel_ = level;
    }
}

void GameResultInfo::RecordSurvivalTime(int32_t seconds) {
    survivalTimeSec_ = seconds;
}

void GameResultInfo::Reset() {
    maxComboCount_   = 0;
    killCount_       = 0;
    reachedLevel_    = 1;
    survivalTimeSec_ = 0;
}

int32_t GameResultInfo::GetMaxComboCount()   const { return maxComboCount_; }
int32_t GameResultInfo::GetTotalKillCount()  const { return killCount_; }
int32_t GameResultInfo::GetReachedLevel()    const { return reachedLevel_; }
int32_t GameResultInfo::GetSurvivalTimeSec() const { return survivalTimeSec_; }
