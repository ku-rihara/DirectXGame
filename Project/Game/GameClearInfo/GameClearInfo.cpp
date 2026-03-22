#include "GameClearInfo.h"

GameClearInfo* GameClearInfo::GetInstance() {
    static GameClearInfo instance;
    return &instance;
}

void GameClearInfo::RecordCombo(int32_t count) {
    if (count > maxComboCount_) {
        maxComboCount_ = count;
    }
}

void GameClearInfo::RecordKillCount(int32_t count) {
    killCount_ = count;
}

void GameClearInfo::RecordLevel(int32_t level) {
    if (level > reachedLevel_) {
        reachedLevel_ = level;
    }
}

void GameClearInfo::Reset() {
    maxComboCount_ = 0;
    killCount_     = 0;
    reachedLevel_  = 1;
}

int32_t GameClearInfo::GetMaxComboCount() const { return maxComboCount_; }
int32_t GameClearInfo::GetTotalKillCount()  const { return killCount_; }
int32_t GameClearInfo::GetReachedLevel()    const { return reachedLevel_; }
