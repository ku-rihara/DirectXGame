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

void GameClearInfo::Reset() {
    maxComboCount_ = 0;
}

int32_t GameClearInfo::GetMaxComboCount() const {
    return maxComboCount_;
}
