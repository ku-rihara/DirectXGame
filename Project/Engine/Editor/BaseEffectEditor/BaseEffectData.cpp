#include "BaseEffectData.h"
#include <imgui.h>

void BaseEffectData::Play() {
    Reset();
    playState_ = PlayState::PLAYING;
}

void BaseEffectData::Pause() {
    if (playState_ == PlayState::PLAYING) {
        playState_ = PlayState::PAUSED;
    } else if (playState_ == PlayState::PAUSED) {
        playState_ = PlayState::PLAYING;
    }
}

void BaseEffectData::InitWithCategory(const std::string& name, const std::string& categoryName) {
    name;
    categoryName;
}

bool BaseEffectData::IsPlaying() const {
    return playState_ == PlayState::PLAYING;
}

bool BaseEffectData::IsFinished() const {
    return playState_ == PlayState::STOPPED;
}

bool BaseEffectData::IsReturning() const {
  return  playState_ == PlayState::RETURNING;
}

void BaseEffectData::LoadData() {
    // ロード、同期
    globalParameter_->LoadFile(groupName_, folderPath_);
    globalParameter_->SyncParamForGroup(groupName_);
    GetParams();
}

void BaseEffectData::SaveData() {
    // セーブ
    globalParameter_->SaveFile(groupName_, folderPath_);
}
