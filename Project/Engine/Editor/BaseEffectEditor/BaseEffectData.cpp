#include "BaseEffectData.h"

using namespace KetaEngine;
#include <imgui.h>

void BaseEffectData::Play() {
    Reset();
    playState_ = PlayState::PLAYING;
}

void BaseEffectData::DrawPlayButton() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    // 再生ボタン共通カラー
    const ImVec4 kPlay        = {0.08f, 0.72f, 0.18f, 1.0f};
    const ImVec4 kPlayHovered = {0.18f, 0.90f, 0.30f, 1.0f};
    const ImVec4 kPlayActive  = {0.04f, 0.55f, 0.10f, 1.0f};

    ImGui::PushStyleColor(ImGuiCol_Button,        kPlay);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, kPlayHovered);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  kPlayActive);

    if (ImGui::Button(("再生プレビュー##" + groupName_).c_str())) {
        Play();
    }

    ImGui::PopStyleColor(3);
#endif
}

void BaseEffectData::Pause() {
    if (playState_ == PlayState::PLAYING) {
        playState_ = PlayState::PAUSED;
    } else if (playState_ == PlayState::PAUSED) {
        playState_ = PlayState::PLAYING;
    }
}

void BaseEffectData::Init(const std::string& name, const std::string& categoryName) {
    groupName_    = name;
    categoryName_ = categoryName;
    globalParameter_ = GlobalParameter::GetInstance();
}

bool BaseEffectData::IsPlaying() const {
    return playState_ == PlayState::PLAYING;
}

bool BaseEffectData::IsFinished() const {
    return playState_ == PlayState::STOPPED;
}

bool BaseEffectData::IsReturning() const {
    return playState_ == PlayState::RETURNING;
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