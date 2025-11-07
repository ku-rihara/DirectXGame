#include "RailData.h"
#include "Frame/Frame.h"
#include "MathFunction.h"
#include <fstream>
#include <imgui.h>
#include <json.hpp>
#include <Windows.h>

using json = nlohmann::json;

void RailData::Init(const std::string& railName) {
    globalParameter_ = GlobalParameter::GetInstance();
    groupName_       = railName;

    // Rail初期化
    rail_ = std::make_unique<Rail>();
    rail_->Init(5);

    // 制御点データをファイルから読み込み
    std::ifstream file(dyrectrypath_ + groupName_ + ".json", std::ios::in);
    if (file.is_open()) {
        json root;
        file >> root;
        file.close();

        controlPoints_.clear();
        for (const auto& position : root) {
            Vector3 pos;
            pos.x = position.at("x").get<float>();
            pos.y = position.at("y").get<float>();
            pos.z = position.at("z").get<float>();
            controlPoints_.push_back(pos);
        }
    }

    if (!globalParameter_->HasRegisters(railName)) {
        // 新規登録
        globalParameter_->CreateGroup(groupName_, true);
        RegisterParams();
        globalParameter_->SyncParamForGroup(groupName_);
    } else {
        // 値取得
        LoadParams();
    }

    // リセット
    ResetParams();
}

void RailData::Update(const float& speed, const PositionMode& mode, const Vector3& direction) {
    if (playState_ != PlayState::PLAYING) {
        return;
    }

    direction_ = direction;

    // 経過時間を更新
    elapsedTime_ += speed;

    // 開始時間に達していない場合は待機
    if (elapsedTime_ < startTime_) {
        currentPosition_ = Vector3::ZeroVector();
        return;
    }

    // レール更新用の座標リストを作成
    std::vector<Vector3> positions;
    for (const auto& point : controlPoints_) {
        Vector3 pos = point * direction_;

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
    rail_->Update(positions);

    // 開始時間を引いた実際のレール移動時間を計算
    float railTime = elapsedTime_ - startTime_;

    // 正規化された時間(0~1)を計算
    float t = 0.0f;
    if (maxTime_ > 0.0f) {
        t = railTime / maxTime_;
    }

    // Easingを適用
    timeEase_.SetType(static_cast<EasingType>(easeType_));
    easedTime_ = timeEase_.EaseInOut(t, 0.0f, 1.0f);

    RoopOrStop();

    // Easing適用後の時間で現在位置を取得
    currentPosition_ = rail_->GetPositionOnRail(easedTime_);
}

void RailData::RoopOrStop() {
    // 開始時間を引いた実際のレール移動時間
    float railTime = elapsedTime_ - startTime_;

    if (railTime < maxTime_)
        return;

    if (isLoop_) {
        elapsedTime_ = 0.0f;
        easedTime_   = 0.0f;
    } else {
        easedTime_ = 1.0f;
        playState_ = PlayState::STOPPED;
    }
}

void RailData::Play() {
    Reset();
    playState_   = PlayState::PLAYING;
    elapsedTime_ = 0.0f;
    easedTime_   = 0.0f;
}

void RailData::Stop() {
    playState_       = PlayState::STOPPED;
    currentPosition_ = Vector3::ZeroVector();
}

void RailData::Reset() {
    elapsedTime_     = 0.0f;
    easedTime_       = 0.0f;
    currentPosition_ = Vector3::ZeroVector();
}

bool RailData::IsFinished() const {
    return playState_ == PlayState::STOPPED;
}

bool RailData::IsPlaying() const {
    return playState_ == PlayState::PLAYING;
}

void RailData::AddPoint(const Vector3& position) {
    controlPoints_.push_back(position);
}

void RailData::RemovePoint(size_t index) {
    if (index < controlPoints_.size()) {
        controlPoints_.erase(controlPoints_.begin() + index);
    }
}

void RailData::LoadData() {
    globalParameter_->LoadFile(groupName_, folderPath_);
    globalParameter_->SyncParamForGroup(groupName_);

    // 制御点データをファイルから読み込み
    std::ifstream file(dyrectrypath_ + groupName_ + ".json", std::ios::in);
    if (file.is_open()) {
        json root;
        file >> root;
        file.close();

        controlPoints_.clear();
        for (const auto& position : root) {
            Vector3 pos;
            pos.x = position.at("x").get<float>();
            pos.y = position.at("y").get<float>();
            pos.z = position.at("z").get<float>();
            controlPoints_.push_back(pos);
        }
    }
}

void RailData::SaveData() {
    globalParameter_->SaveFile(groupName_, folderPath_);

    // 制御点データをファイルに保存
    json root;
    for (const auto& pos : controlPoints_) {
        root.push_back({{"x", pos.x}, {"y", pos.y}, {"z", pos.z}});
    }

    std::ofstream file(dyrectrypath_ + groupName_ + ".json", std::ios::out);
    if (file.is_open()) {
        file << root.dump(4);
        file.close();
    }
}

void RailData::RegisterParams() {
    globalParameter_->Regist(groupName_, "maxTime", &maxTime_);
    globalParameter_->Regist(groupName_, "startTime", &startTime_);
    globalParameter_->Regist(groupName_, "isLoop", &isLoop_);
    globalParameter_->Regist(groupName_, "easeType", &easeType_);
}

void RailData::LoadParams() {
    maxTime_   = globalParameter_->GetValue<float>(groupName_, "maxTime");
    startTime_ = globalParameter_->GetValue<float>(groupName_, "startTime");
    isLoop_    = globalParameter_->GetValue<bool>(groupName_, "isLoop");
    easeType_  = globalParameter_->GetValue<int32_t>(groupName_, "easeType");
}

void RailData::ResetParams() {
    playState_       = PlayState::STOPPED;
    currentPosition_ = Vector3::ZeroVector();
    elapsedTime_     = 0.0f;
    easedTime_       = 0.0f;

    // Easing初期化
    timeEase_.SetAdaptValue(&easedTime_);
}

void RailData::ImGuiControlPoints() {
    // 色を緑系統に変更
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));

    if (ImGui::CollapsingHeader("Control Points")) {
        // 現在の座標リストを表示
        ImGui::SeparatorText("Current Positions");
        for (size_t i = 0; i < controlPoints_.size(); ++i) {
            ImGui::PushID(static_cast<int>(i));
            if (ImGui::DragFloat3("Edit Position", &controlPoints_[i].x, 0.1f)) {
            }
            ImGui::SameLine();
            if (ImGui::Button("Remove")) {
                RemovePoint(i);
            }
            ImGui::PopID();
        }

        // 座標追加用
        ImGui::SeparatorText("Add New Position");
        ImGui::DragFloat3("New Position", &tempAddPosition_.x, 0.1f);
        if (ImGui::Button("Add Position")) {
            AddPoint(tempAddPosition_);
            tempAddPosition_ = {0.0f, 0.0f, 0.0f};
        }
    }

    ImGui::PopStyleColor();
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
        }
        ImGui::Text("State: %s", stateText);

        // 進行状況を表示
        float progress  = 0.0f;
        float totalTime = startTime_ + maxTime_;
        if (totalTime > 0.0f) {
            progress = elapsedTime_ / totalTime;
        }
        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f), "Progress");

        ImGui::Separator();

        // パラメータ調整
        ImGui::DragFloat("Start Time", &startTime_, 0.01f, 0.0f, 10.0f);
        ImGui::DragFloat("Max Time", &maxTime_, 0.01f, 0.1f, 10.0f);
        ImGui::Checkbox("Loop", &isLoop_);

        ImGui::Separator();

        // イージングタイプ選択
        ImGuiEasingTypeSelector("Easing Type", easeType_);

        ImGui::Separator();

        // 制御点エディタ
        ImGuiControlPoints();

        ImGui::PopID();
    }
#endif // _DEBUG
}