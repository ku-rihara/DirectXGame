#include "ObjEaseAnimationData.h"
#include "3d/WorldTransform.h"
#include "Editor/RailEditor/RailPlayer.h"
#include "MathFunction.h"
#include <algorithm>
#include <imgui.h>

void ObjEaseAnimationData::Init(const std::string& animationName, const std::string& categoryName) {
    globalParameter_ = GlobalParameter::GetInstance();
    groupName_       = animationName;
    categoryName_    = categoryName;

    // DatesとKeyFramesフォルダパス設定
    folderPath_         = baseFolderPath_ + categoryName_ + "/" + "Dates";
    keyFrameFolderPath_ = baseFolderPath_ + categoryName_ + "/" + "KeyFrames/" + animationName + "/";

    if (!globalParameter_->HasRegisters(groupName_)) {
        // 新規登録
        globalParameter_->CreateGroup(groupName_);
        RegisterParams();
        globalParameter_->SyncParamForGroup(groupName_);
    } else {
        // 値取得
        GetParams();
    }

    // リセット
    ResetParams();
}

void ObjEaseAnimationData::Update() {

    UpdateKeyFrameProgression();
}

void ObjEaseAnimationData::UpdateKeyFrameProgression() {
    if (keyFrameDates_.empty() || playState_ != PlayState::PLAYING) {
        return;
    }

    // 現在のキーフレームが完了したかチェック
    if (keyFrameState_.activeKeyFrameIndex >= 0 && keyFrameState_.activeKeyFrameIndex < static_cast<int32_t>(keyFrameDates_.size())) {
        if (!keyFrameDates_[keyFrameState_.activeKeyFrameIndex]->IsFinished()) {
            return;
        }

        keyFrameState_.lastCompletedKeyFrameIndex = keyFrameState_.activeKeyFrameIndex;

        // 最後のキーフレームかチェック
        if (keyFrameState_.activeKeyFrameIndex == static_cast<int32_t>(keyFrameDates_.size()) - 1) {
            keyFrameState_.isAllKeyFramesFinished = true;
            keyFrameState_.finalKeyFrameIndex     = keyFrameState_.activeKeyFrameIndex;

            playState_ = PlayState::STOPPED;

        } else {
            AdvanceToNextKeyFrame();
        }
    }
}

void ObjEaseAnimationData::AdvanceToNextKeyFrame() {
    if (keyFrameState_.activeKeyFrameIndex < static_cast<int32_t>(keyFrameDates_.size()) - 1) {
        keyFrameState_.activeKeyFrameIndex++;

        if (keyFrameState_.activeKeyFrameIndex < static_cast<int32_t>(keyFrameDates_.size())) {
            // 前のキーフレームの最終値を次のキーフレームの開始値として設定
            Vector3 startScale       = GetActiveKeyFrameValue(TransformType::Scale);
            Vector3 startRotation    = GetActiveKeyFrameValue(TransformType::Rotation);
            Vector3 startTranslation = GetActiveKeyFrameValue(TransformType::Translation);

            keyFrameDates_[keyFrameState_.activeKeyFrameIndex]->SetStartValues(startScale, startRotation, startTranslation);
        }
    }
}

void ObjEaseAnimationData::Play() {
    Reset();
    playState_ = PlayState::PLAYING;
}

void ObjEaseAnimationData::Stop() {
    playState_ = PlayState::STOPPED;
}

void ObjEaseAnimationData::Reset() {
}

bool ObjEaseAnimationData::IsFinished() const {
    return playState_ == PlayState::STOPPED;
}

bool ObjEaseAnimationData::IsPlaying() const {
    return playState_ == PlayState::PLAYING || playState_ == PlayState::RETURNING;
}

void ObjEaseAnimationData::LoadData() {
    globalParameter_->LoadFile(groupName_, folderPath_);
    LoadKeyFrames();
    globalParameter_->SyncParamForGroup(groupName_);
    GetParams();
}

void ObjEaseAnimationData::LoadKeyFrames() {
    std::string folderPath     = globalParameter_->GetDirectoryPath() + ""+ keyFrameFolderPath_;
    std::string keyFramePrefix = groupName_;

    if (std::filesystem::exists(folderPath) && std::filesystem::is_directory(folderPath)) {

        std::vector<std::pair<int32_t, std::string>> keyFrameFiles;

        // キーフレームファイルを検索
        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                std::string fileName = entry.path().stem().string();

                // ファイルチェック
                if (fileName.find(keyFramePrefix) == 0) {
                    // インデックス番号を抽出
                    std::string indexStr = fileName.substr(keyFramePrefix.length());

                    int32_t index = std::stoi(indexStr);
                    keyFrameFiles.emplace_back(index, fileName);
                }
            }
        }

        // インデックス順にソート
        std::sort(keyFrameFiles.begin(), keyFrameFiles.end());

        CreateOrLoadKeyFrames(keyFrameFiles);
    }
}

void ObjEaseAnimationData::CreateOrLoadKeyFrames(const std::vector<std::pair<int32_t, std::string>>& KeyFrameFiles) {

    // 作成
    if (keyFrameDates_.size() == 0) {
        // 既存のキーフレームをクリア
        ClearKeyFrames();
        // キーフレームを作成してロード
        for (const auto& [index, fileName] : KeyFrameFiles) {
            auto newKeyFrame = std::make_unique<ObjEaseAnimationKeyFrame>();
            newKeyFrame->Init(groupName_, categoryName_, index);
            newKeyFrame->LoadData(); // Load
            keyFrameDates_.push_back(std::move(newKeyFrame));
        }
    } else {

        // すべてのキーフレームを保存
        for (auto& keyFrame : keyFrameDates_) {
            keyFrame->LoadData();
        }
    }
}

void ObjEaseAnimationData::SaveData() {
    globalParameter_->SaveFile(groupName_, folderPath_);
    SaveKeyFrames();
}

void ObjEaseAnimationData::SaveKeyFrames() {
    // すべてのキーフレームを保存
    for (auto& keyFrame : keyFrameDates_) {
        keyFrame->SaveData();
    }
}

void ObjEaseAnimationData::RegisterParams() {
}

void ObjEaseAnimationData::GetParams() {
}

void ObjEaseAnimationData::ResetParams() {
    playState_                         = PlayState::STOPPED;
    keyFrameState_.activeKeyFrameIndex = 0;

    keyFrameState_.isAllKeyFramesFinished     = false;
    keyFrameState_.lastCompletedKeyFrameIndex = -1;
}

void ObjEaseAnimationData::AdjustParam() {
#ifdef _DEBUG
    if (showControls_) {
        ImGui::SeparatorText(("Animation: " + groupName_).c_str());
        ImGui::PushID(groupName_.c_str());

        // 再生制御
        if (ImGui::Button("Play"))
            Play();
        ImGui::SameLine();
        if (ImGui::Button("Stop"))
            Stop();
        ImGui::SameLine();
        if (ImGui::Button("Reset"))
            Reset();

        // 状態表示
        const char* stateText = "";
        switch (playState_) {
        case PlayState::STOPPED:
            stateText = "STOPPED";
            break;
        case PlayState::PLAYING:
            stateText = "PLAYING";
            break;
        case PlayState::RETURNING:
            stateText = "RETURNING";
            break;
        }
        ImGui::Text("State: %s", stateText);

        if (keyFrameState_.isAllKeyFramesFinished) {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Animation Finished!");
        }

        ImGui::PopID();
    }

    // キーフレームリスト

    ImGui::SeparatorText("KeyFrames");
    ImGui::Text("KeyFrames (%zu):", keyFrameDates_.size());

    for (int32_t i = 0; i < static_cast<int32_t>(keyFrameDates_.size()); ++i) {
        ImGui::PushID(i);

        bool isSelected       = (keyFrameState_.selectedKeyFrameIndex == i);
        bool isActive         = (keyFrameState_.activeKeyFrameIndex == i);
        std::string labelText = "KeyFrame " + std::to_string(i) + " (t:" + std::to_string(keyFrameDates_[i]->GetTimePoint()) + ")";

        if (isActive) {
            labelText += " [ACTIVE]";
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
        }

        if (ImGui::Selectable(labelText.c_str(), isSelected, 0, ImVec2(0, 0))) {
            keyFrameState_.selectedKeyFrameIndex = i;
        }

        if (isActive) {
            ImGui::PopStyleColor();
        }

        ImGui::PopID();
        ImGui::Spacing();
    }

    if (ImGui::Button("Add KeyFrame")) {
        AddKeyFrame();
    }
    ImGui::SameLine();
    if (ImGui::Button("Remove Selected KeyFrame")) {
        if (keyFrameState_.selectedKeyFrameIndex >= 0 && keyFrameState_.selectedKeyFrameIndex < static_cast<int32_t>(keyFrameDates_.size())) {
            RemoveKeyFrame(keyFrameState_.selectedKeyFrameIndex);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Clear All KeyFrames")) {
        ClearKeyFrames();
    }

    ImGui::Separator();

    // 選択されたキーフレームの調整
    if (keyFrameState_.selectedKeyFrameIndex >= 0 && keyFrameState_.selectedKeyFrameIndex < static_cast<int32_t>(keyFrameDates_.size())) {
        keyFrameDates_[keyFrameState_.selectedKeyFrameIndex]->AdjustParam();
    }
#endif
}

void ObjEaseAnimationData::ClearKeyFrames() {
    keyFrameDates_.clear();
    keyFrameState_.selectedKeyFrameIndex = -1;
    keyFrameState_.activeKeyFrameIndex   = 0;

    // フラグをリセット
    keyFrameState_.isAllKeyFramesFinished = false;

    keyFrameState_.lastCompletedKeyFrameIndex = -1;
}

void ObjEaseAnimationData::AddKeyFrame() {
    int32_t newIndex = static_cast<int32_t>(keyFrameDates_.size());
    auto newKeyFrame = std::make_unique<ObjEaseAnimationKeyFrame>();
    newKeyFrame->Init(groupName_, categoryName_, newIndex);

    keyFrameDates_.push_back(std::move(newKeyFrame));
    keyFrameState_.selectedKeyFrameIndex = newIndex;

    // フラグをリセット
    keyFrameState_.isAllKeyFramesFinished = false;
}

void ObjEaseAnimationData::RemoveKeyFrame(const int32_t& index) {
    if (index >= 0 && index < static_cast<int32_t>(keyFrameDates_.size())) {
        keyFrameDates_.erase(keyFrameDates_.begin() + index);

        // 選択インデックスの調整
        if (keyFrameState_.selectedKeyFrameIndex >= index) {
            keyFrameState_.selectedKeyFrameIndex--;
            if (keyFrameState_.selectedKeyFrameIndex < 0 && !keyFrameDates_.empty()) {
                keyFrameState_.selectedKeyFrameIndex = 0;
            }
        }

        // アクティブインデックスの調整
        if (keyFrameState_.activeKeyFrameIndex >= index) {
            keyFrameState_.activeKeyFrameIndex--;
            if (keyFrameState_.activeKeyFrameIndex < 0 && !keyFrameDates_.empty()) {
                keyFrameState_.activeKeyFrameIndex = 0;
            }
        }

        // インデックスの再設定
        for (int32_t i = 0; i < static_cast<int32_t>(keyFrameDates_.size()); ++i) {
            keyFrameDates_[i]->Init(groupName_, categoryName_, i);
        }

        // フラグをリセット
        keyFrameState_.isAllKeyFramesFinished = false;

        keyFrameState_.lastCompletedKeyFrameIndex = -1;
    }
}

//*-------------------------------getter Method-------------------------------*/

const char* ObjEaseAnimationData::GetSRTName(const TransformType& type) const {
    switch (type) {
    case TransformType::Scale:
        return "Scale";
    case TransformType::Rotation:
        return "Rotation";
    case TransformType::Translation:
        return "Translation";
    default:
        return "Unknown";
    }
}

RailPlayer* ObjEaseAnimationData::GetCurrentRailPlayer() const {
    if (keyFrameState_.activeKeyFrameIndex >= 0 && keyFrameState_.activeKeyFrameIndex < static_cast<int32_t>(keyFrameDates_.size())) {
        auto* railPlayer = keyFrameDates_[keyFrameState_.activeKeyFrameIndex]->GetRailPlayer();
        if (railPlayer) {
            return railPlayer;
        }
    }
    return nullptr;
}

Vector3 ObjEaseAnimationData::GetActiveKeyFrameValue(const TransformType& type) const {
    if (keyFrameDates_.empty()) {
        return originalValues_[static_cast<size_t>(type)];
    }

    // 現在のアクティブキーフレームを更新
    if (keyFrameState_.activeKeyFrameIndex >= 0 && keyFrameState_.activeKeyFrameIndex < static_cast<int32_t>(keyFrameDates_.size())) {

        switch (type) {
            // scale
        case TransformType::Scale:
            return keyFrameDates_[keyFrameState_.activeKeyFrameIndex]->GetCurrentScale();
            break;

            // rotation
        case TransformType::Rotation:
            return keyFrameDates_[keyFrameState_.activeKeyFrameIndex]->GetCurrentRotation();

            break;
            // translation
        case TransformType::Translation:
            return keyFrameDates_[keyFrameState_.activeKeyFrameIndex]->GetCurrentTranslation();

            break;
        default:
            break;
        }
    }

    return Vector3::ZeroVector();
}

bool ObjEaseAnimationData::GetIsUseRailActiveKeyFrame() const {
    bool isUseRail = false;
    if (keyFrameState_.activeKeyFrameIndex >= 0 && keyFrameState_.activeKeyFrameIndex < static_cast<int32_t>(keyFrameDates_.size())) {
        isUseRail = keyFrameDates_[keyFrameState_.activeKeyFrameIndex]->IsUsingRail();
    }

    return isUseRail;
}