#include "RailData.h"
#include "Frame/Frame.h"
#include "Line3D/Line3D.h"
#include "MathFunction.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <imgui.h>
#include <Windows.h>

void RailData::Init(const std::string& railName) {
    globalParameter_ = GlobalParameter::GetInstance();
    groupName_       = railName;

    // Rail初期化
    rail_ = std::make_unique<Rail>();
    rail_->Init(5);

    if (!globalParameter_->HasRegisters(railName)) {
        // 新規登録
        globalParameter_->CreateGroup(groupName_);
        RegisterParams();
        globalParameter_->SyncParamForGroup(groupName_);
    } else {
        // 値取得
        LoadParams();
    }

    // リセット
    InitParams();
}

void RailData::Update(const float& speed, const PositionMode& mode, const Vector3& direction) {
    // 戻り中の更新処理
    if (playState_ == PlayState::RETURNING) {
        UpdateReturn(speed);
        return;
    }

    if (playState_ != PlayState::PLAYING) {
        return;
    }

    direction_ = direction;

    // Easingを更新
    railMoveParam_.timeEase.Update(speed);

    // 開始時間に達していない場合は待機
    if (railMoveParam_.elapsedTime < railMoveParam_.startTime) {
        currentPosition_ = startPosition_; // 開始位置を保持
        return;
    }

    // キーフレームから制御点リストを作成
    std::vector<Vector3> positions;
    for (const auto& keyFrame : controlPoints_) {
        Vector3 pos = keyFrame->GetPosition() * direction_;

        if (mode == PositionMode::WORLD && parentTransform_ != nullptr) {
            // ワールド座標に変換
            Matrix4x4 parentMatrix = parentTransform_->matWorld_;
            Vector3 worldPos       = TransformMatrix(pos, parentMatrix);
            positions.push_back(worldPos);
        } else {
            positions.push_back(pos);
        }
    }

    // レール更新
    if (!positions.empty()) {
        rail_->Update(positions);
    }

    CheckAndHandleFinish();

    // Easing適用後の時間で現在位置を取得
    currentPosition_ = startPosition_ + rail_->GetPositionOnRail(railMoveParam_.adaptTime);
}

void RailData::CheckAndHandleFinish() {
    // イージングが完了したかチェック
    if (!railMoveParam_.timeEase.IsFinished()) {
        return;
    }

    railMoveParam_.adaptTime = 1.0f;

    // 戻りモードが設定されている場合、戻り動作を開始
    if (railMoveParam_.returnMode != ReturnMode::NONE) {
        StartReturn();
    } else {
        // 戻りモードがNONEの場合、ループ判定
        if (railMoveParam_.isLoop) {
            railMoveParam_.timeEase.Reset();
            railMoveParam_.adaptTime = 0.0f;
        } else {
            playState_ = PlayState::STOPPED;
        }
    }
}

void RailData::StartReturn() {
    playState_                      = PlayState::RETURNING;
    directReturnParam_.easeAdaptPos = Vector3::ZeroVector();
    railMoveParam_.timeEase.Reset();

    if (railMoveParam_.returnMode == ReturnMode::DIRECT_RETURN) {
        // 直接戻る用のイージング設定
        directReturnParam_.ease.SetAdaptValue(&directReturnParam_.easeAdaptPos);
        directReturnParam_.ease.SetStartValue(currentPosition_);
        directReturnParam_.ease.SetEndValue(startPosition_);
        directReturnParam_.ease.SetMaxTime(directReturnParam_.maxTime);
        directReturnParam_.ease.SetType(static_cast<EasingType>(directReturnParam_.easeTypeInt));
        directReturnParam_.ease.SetIsStartEndReverse(false);
        directReturnParam_.ease.Reset();
    } else if (railMoveParam_.returnMode == ReturnMode::REVERSE_RAIL) {

        railMoveParam_.timeEase.SetIsStartEndReverse(true);
        railMoveParam_.timeEase.SetMaxTime(directReturnParam_.maxTime);
        railMoveParam_.timeEase.SetType(static_cast<EasingType>(directReturnParam_.easeTypeInt));
    }
}

void RailData::UpdateReturn(const float& speed) {
    switch (railMoveParam_.returnMode) {
    case ReturnMode::REVERSE_RAIL:
        // レールを逆走
        railMoveParam_.timeEase.Update(speed);

        // easedTimeが0以下になったら終了
        if (railMoveParam_.adaptTime <= 0.0f) {
            railMoveParam_.adaptTime       = 0.0f;
            currentPosition_ = startPosition_;

            // 戻り完了後の処理
            OnReturnComplete();
        } else {
            currentPosition_ = startPosition_ + rail_->GetPositionOnRail(railMoveParam_.adaptTime);
        }
        break;

    case ReturnMode::DIRECT_RETURN:
        // 直接戻る
        directReturnParam_.ease.Update(speed);

        currentPosition_ = directReturnParam_.easeAdaptPos;

        if (directReturnParam_.ease.IsFinished()) {
            currentPosition_ = startPosition_;

            // 戻り完了後の処理
            OnReturnComplete();
        }
        break;

    default:
        break;
    }
}

void RailData::OnReturnComplete() {
    // 戻り動作完了後の処理
    if (railMoveParam_.isLoop) {
        // ループする場合は再度再生開始
        Play();
    } else {
        // ループしない場合は停止
        playState_ = PlayState::STOPPED;
        // 元のイージング設定に戻す
        railMoveParam_.timeEase.SetIsStartEndReverse(false);
        railMoveParam_.timeEase.SetMaxTime(railMoveParam_.maxTime);
        railMoveParam_.timeEase.SetType(static_cast<EasingType>(railMoveParam_.easeTypeInt));
    }
}

void RailData::Play() {
    Reset();
    startPosition_ = currentPosition_;
    playState_     = PlayState::PLAYING;
    railMoveParam_.timeEase.Reset();
}

void RailData::Stop() {
    playState_       = PlayState::STOPPED;
    currentPosition_ = startPosition_;
}

void RailData::Reset() {
    railMoveParam_.elapsedTime     = 0.0f;
    railMoveParam_.adaptTime       = 0.0f;
    currentPosition_ = startPosition_;
    railMoveParam_.timeEase.Reset();
}

bool RailData::IsFinished() const {
    return playState_ == PlayState::STOPPED;
}

bool RailData::IsPlaying() const {
    return playState_ == PlayState::PLAYING || playState_ == PlayState::RETURNING;
}

void RailData::AddKeyFrame() {
    int32_t newIndex = static_cast<int32_t>(controlPoints_.size());
    auto newKeyFrame = std::make_unique<RailControlPoint>();
    newKeyFrame->Init(groupName_, newIndex);

    controlPoints_.push_back(std::move(newKeyFrame));
    selectedKeyFrameIndex_ = newIndex;
}

void RailData::RemoveKeyFrame(const int32_t& index) {
    if (index >= 0 && index < static_cast<int32_t>(controlPoints_.size())) {
        controlPoints_.erase(controlPoints_.begin() + index);

        // 選択インデックスの調整
        if (selectedKeyFrameIndex_ >= index) {
            selectedKeyFrameIndex_--;
            if (selectedKeyFrameIndex_ < 0 && !controlPoints_.empty()) {
                selectedKeyFrameIndex_ = 0;
            }
        }

        // インデックスの再設定
        for (int32_t i = 0; i < static_cast<int32_t>(controlPoints_.size()); ++i) {
            controlPoints_[i]->Init(groupName_, i);
        }
    }
}

void RailData::ClearKeyFrames() {
    controlPoints_.clear();
    selectedKeyFrameIndex_ = -1;
}

void RailData::InitKeyFrames() {
    // すべてのキーフレームを初期化
    for (int32_t i = 0; i < static_cast<int32_t>(controlPoints_.size()); ++i) {
        controlPoints_[i]->Init(groupName_, i);
    }
}

void RailData::LoadData() {
    globalParameter_->LoadFile(groupName_, folderPath_);
    globalParameter_->SyncParamForGroup(groupName_);

    // キーフレームデータの読み込み
    LoadKeyFrames();
}

void RailData::SaveData() {
    globalParameter_->SaveFile(groupName_, folderPath_);

    // キーフレームデータの保存
    SaveKeyFrames();
}

void RailData::SaveKeyFrames() {
    // すべてのキーフレームを保存
    for (auto& keyFrame : controlPoints_) {
        keyFrame->SaveData();
    }
}

void RailData::LoadKeyFrames() {
    std::string folderPath     = globalParameter_->GetDirectoryPath() + "RailEditor/ControlPoints/";
    std::string keyFramePrefix = groupName_;

    if (std::filesystem::exists(folderPath) && std::filesystem::is_directory(folderPath)) {

        std::vector<std::pair<int32_t, std::string>> keyFrameFiles;

        // キーフレームファイルを検索
        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                std::string fileName = entry.path().stem().string();

                // ファイル名チェック
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

        RebuildAndLoadAllKeyFrames(keyFrameFiles);

        // 最初のキーフレームを選択状態に
        if (!controlPoints_.empty()) {
            selectedKeyFrameIndex_ = 0;
        }
    }
}

void RailData::RebuildAndLoadAllKeyFrames(const std::vector<std::pair<int32_t, std::string>>& KeyFrameFiles) {
    // 再構築
    if (controlPoints_.size() == 0) {
        // 既存のキーフレームをクリア
        ClearKeyFrames();
        // キーフレームを作成してロード
        for (const auto& [index, fileName] : KeyFrameFiles) {
            auto newKeyFrame = std::make_unique<RailControlPoint>();
            newKeyFrame->Init(groupName_, index);
            newKeyFrame->LoadData();
            controlPoints_.push_back(std::move(newKeyFrame));
        }
    } else {
        // すべてのキーフレームをロード
        for (auto& keyFrame : controlPoints_) {
            keyFrame->LoadData();
        }
    }
}

void RailData::RegisterParams() {
    globalParameter_->Regist(groupName_, "maxTime", &railMoveParam_.maxTime);
    globalParameter_->Regist(groupName_, "startTime", &railMoveParam_.startTime);
    globalParameter_->Regist(groupName_, "isLoop", &railMoveParam_.isLoop);
    globalParameter_->Regist(groupName_, "easeType", &railMoveParam_.easeTypeInt);
    globalParameter_->Regist(groupName_, "returnMode", &railMoveParam_.returnModeInt);
    globalParameter_->Regist(groupName_, "returnTime", &directReturnParam_.maxTime);
    globalParameter_->Regist(groupName_, "returnEaseType", &directReturnParam_.easeTypeInt);
}

void RailData::LoadParams() {
    railMoveParam_.maxTime              = globalParameter_->GetValue<float>(groupName_, "maxTime");
    railMoveParam_.startTime            = globalParameter_->GetValue<float>(groupName_, "startTime");
    railMoveParam_.isLoop               = globalParameter_->GetValue<bool>(groupName_, "isLoop");
    railMoveParam_.easeTypeInt             = globalParameter_->GetValue<int32_t>(groupName_, "easeType");
    railMoveParam_.returnModeInt  = globalParameter_->GetValue<int32_t>(groupName_, "returnMode");
    directReturnParam_.maxTime  = globalParameter_->GetValue<float>(groupName_, "returnTime");
    directReturnParam_.easeTypeInt = globalParameter_->GetValue<int32_t>(groupName_, "returnEaseType");
    railMoveParam_.returnMode     = static_cast<ReturnMode>(railMoveParam_.returnModeInt);
}

void RailData::InitParams() {
    playState_                 = PlayState::STOPPED;
    currentPosition_           = Vector3::ZeroVector();
    startPosition_             = Vector3::ZeroVector();
    railMoveParam_.elapsedTime = 0.0f;
    railMoveParam_.adaptTime   = 0.0f;

    // Easing初期化
    railMoveParam_.timeEase.SetAdaptValue(&railMoveParam_.adaptTime);
    railMoveParam_.timeEase.SetStartValue(0.0f);
    railMoveParam_.timeEase.SetEndValue(1.0f);
    railMoveParam_.timeEase.SetMaxTime(railMoveParam_.maxTime);
    railMoveParam_.timeEase.SetType(static_cast<EasingType>(railMoveParam_.easeTypeInt));
    railMoveParam_.timeEase.SetIsStartEndReverse(false);

    railMoveParam_.returnMode = static_cast<ReturnMode>(railMoveParam_.returnModeInt);
    railMoveParam_.timeEase.Reset();
}

void RailData::ImGuiKeyFrameList() {
    ImGui::SeparatorText("KeyFrames");

    if (ImGui::Button("Add KeyFrame")) {
        AddKeyFrame();
    }
    ImGui::SameLine();
    if (ImGui::Button("Clear All KeyFrames")) {
        ClearKeyFrames();
    }

    ImGui::Separator();

    for (int32_t i = 0; i < static_cast<int32_t>(controlPoints_.size()); ++i) {
        ImGui::PushID(i);
        controlPoints_[i]->AdjustParam();
        ImGui::PopID();
    }
}

void RailData::AdjustParam() {
#ifdef _DEBUG
    if (showControls_) {
        ImGui::SeparatorText(("Rail Editor: " + groupName_).c_str());
        ImGui::PushID(groupName_.c_str());

        // 再生制御
        if (ImGui::Button("Play")) {
            Play();
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop")) {
            Stop();
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset")) {
            Reset();
        }

        // 状態表示
        const char* stateText = "";
        switch (playState_) {
        case PlayState::STOPPED:
            stateText = "STOPPED";
            break;
        case PlayState::PLAYING:
            stateText = "PLAYING";
            break;
        case PlayState::PAUSED:
            stateText = "PAUSED";
            break;
        case PlayState::RETURNING:
            stateText = "RETURNING";
            break;
        }
        ImGui::Text("State: %s", stateText);

        // 進行状況を表示
        float progress = railMoveParam_.adaptTime;
        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f), "Progress");

        ImGui::Separator();

        // パラメータ調整
        ImGui::DragFloat("Start Time", &railMoveParam_.startTime, 0.01f, 0.0f, 10.0f);
        ImGui::DragFloat("Max Time", &railMoveParam_.maxTime, 0.01f, 0.1f, 10.0f);
        ImGui::Checkbox("Loop", &railMoveParam_.isLoop);

        // ループの説明を追加
        if (railMoveParam_.returnMode != ReturnMode::NONE) {
            ImGui::TextWrapped("Note: Loop will restart after return completes");
        }

        ImGui::Separator();

        // イージングタイプ選択
        ImGuiEasingTypeSelector("Easing Type", railMoveParam_.easeTypeInt);

        // パラメータをEasingに反映
        railMoveParam_.timeEase.SetMaxTime(railMoveParam_.maxTime);
        railMoveParam_.timeEase.SetType(static_cast<EasingType>(railMoveParam_.easeTypeInt));

        ImGui::Separator();

        // 戻りモードの設定
        ImGui::SeparatorText("Return Settings");
        const char* returnModes[] = {"None", "Reverse Rail", "Direct Return"};
        if (ImGui::Combo("Return Mode", &railMoveParam_.returnModeInt, returnModes, IM_ARRAYSIZE(returnModes))) {
            railMoveParam_.returnMode = static_cast<ReturnMode>(railMoveParam_.returnModeInt);
        }

        // 戻り用の設定を表示
        if (railMoveParam_.returnMode != ReturnMode::NONE) {
            ImGui::DragFloat("Return Time", &directReturnParam_.maxTime, 0.01f, 0.1f, 10.0f);
            ImGuiEasingTypeSelector("Return Easing Type", directReturnParam_.easeTypeInt);
            ImGui::TextWrapped("Return is part of one complete cycle");
        }

        ImGui::Separator();

        // キーフレームリスト
        if (showKeyFrameList_) {
            ImGuiKeyFrameList();
        }

        ImGui::PopID();
    }
#endif // _DEBUG
}

void RailData::SetControlPointLines(Line3D* line3d, const Vector4& color) {
    if (!line3d || !showControlPointLines_) {
        return;
    }

    if (controlPoints_.size() < 2) {
        return; // 制御点が2つ未満なら線を描画しない
    }

    // 制御点を繋ぐ線を描画
    for (size_t i = 0; i < controlPoints_.size() - 1; ++i) {
        Vector3 start = controlPoints_[i]->GetPosition() * direction_;
        Vector3 end   = controlPoints_[i + 1]->GetPosition() * direction_;

        // ワールド座標に変換
        if (parentTransform_ != nullptr) {
            Matrix4x4 parentMatrix = parentTransform_->matWorld_;
            start                  = TransformMatrix(start, parentMatrix);
            end                    = TransformMatrix(end, parentMatrix);
        }

        // 開始位置をオフセット
        start = startPosition_ + start;
        end   = startPosition_ + end;

        line3d->SetLine(start, end, color);
    }
}

void RailData::EaseTimeSetup(const bool& isReverse) {
    if (isReverse) {
        railMoveParam_.timeEase.SetStartValue(1.0f);
        railMoveParam_.timeEase.SetEndValue(0.0f);
    } else {
        railMoveParam_.timeEase.SetStartValue(0.0f);
        railMoveParam_.timeEase.SetEndValue(1.0f);
    }
 }