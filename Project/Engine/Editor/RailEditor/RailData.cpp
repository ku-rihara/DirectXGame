#include "RailData.h"

using namespace KetaEngine;
#include "Frame/Frame.h"
#include "Line3D/Line3D.h"
#include "MathFunction.h"
#include <algorithm>
#include <imgui.h>

void RailData::Init(const std::string& railName) {
    BaseSequenceEffectData::Init(railName);

    folderPath_ = dateFolderPath_;

    rail_ = std::make_unique<Rail>();
    rail_->Init(5);

    if (!globalParameter_->HasRegisters(railName)) {
        globalParameter_->CreateGroup(groupName_);
        RegisterParams();
        globalParameter_->SyncParamForGroup(groupName_);
    } else {
        GetParams();
    }

    InitParams();
}

void RailData::Update(float speedRate) {
    UpdateWithDirection(speedRate, PositionMode::WORLD, {1.0f, 1.0f, 1.0f});
}

void RailData::UpdateWithDirection(float speedRate, const PositionMode& mode, const Vector3& direction) {
    if (playState_ == PlayState::RETURNING) {
        UpdateReturn(speedRate);
        return;
    }

    if (playState_ != PlayState::PLAYING) {
        return;
    }

    direction_ = direction;

    // 前の位置を保存
    Vector3 previousPos = currentPosition_;

    // 実際のデルタタイムを計算
    float actualDeltaTime = 0.0f;
    switch (static_cast<TimeMode>(timeModeSelector_.GetTimeModeInt())) {
    case TimeMode::DELTA_TIME:
        actualDeltaTime = Frame::DeltaTime() * speedRate;
        break;
    case TimeMode::DELTA_TIME_RATE:
        actualDeltaTime = Frame::DeltaTimeRate() * speedRate;
        break;
    default:
        break;
    }

    railMoveParam_.timeEase.Update(actualDeltaTime);

    if (railMoveParam_.elapsedTime < railMoveParam_.startTime) {
        currentPosition_ = startPosition_;
        return;
    }

    std::vector<Vector3> positions;
    for (const auto& keyFrame : sectionElements_) {
        Vector3 pos = keyFrame->GetPosition() * direction_;

        if (mode == PositionMode::WORLD && parentTransform_ != nullptr) {
            Matrix4x4 parentMatrix = parentTransform_->matWorld_;
            Vector3 worldPos       = TransformMatrix(pos, parentMatrix);
            positions.push_back(worldPos);
        } else {
            positions.push_back(pos);
        }
    }

    if (!positions.empty()) {
        rail_->Update(positions);
    }

    currentPosition_ = startPosition_ + rail_->GetPositionOnRail(railMoveParam_.adaptTime);

     if (railMoveParam_.isLookAtDirection && parentTransform_) {
        Vector3 moveDirection = currentPosition_ - previousPos;
        if (moveDirection.Length() > 0.001f) {
            parentTransform_->ApplyLookAtDirection(moveDirection);
        }
    }


    CheckAndHandleFinish();
}

void RailData::CheckAndHandleFinish() {
    if (!railMoveParam_.timeEase.IsFinished()) {
        return;
    }

    railMoveParam_.adaptTime = 1.0f;

    if (railMoveParam_.returnMode != ReturnMode::NONE) {
        StartReturn();
    } else {
        if (railMoveParam_.isLoop) {
            railMoveParam_.timeEase.Reset();
            railMoveParam_.adaptTime = 0.0f;
        } else {
            playState_ = PlayState::STOPPED;
        }
    }
}

void RailData::StartReturn() {
    playState_ = PlayState::RETURNING;
    railMoveParam_.timeEase.Reset();

    if (railMoveParam_.returnMode == ReturnMode::DIRECT_RETURN) {

        directReturnParam_.easeAdaptPos = Vector3::ZeroVector();
        directReturnParam_.ease.SetAdaptValue(&directReturnParam_.easeAdaptPos);

        // 終点の位置を計算
        Vector3 endPosition = startPosition_ + rail_->GetPositionOnRail(1.0f);
        directReturnParam_.ease.SetStartValue(endPosition);
        directReturnParam_.ease.SetEndValue(startPosition_);
        directReturnParam_.ease.SetMaxTime(directReturnParam_.maxTime);
        directReturnParam_.ease.SetType(static_cast<EasingType>(directReturnParam_.easeTypeInt));
        directReturnParam_.ease.SetIsStartEndReverse(false);
        directReturnParam_.ease.Reset();

        currentPosition_ = endPosition;
    } else if (railMoveParam_.returnMode == ReturnMode::REVERSE_RAIL) {

        railMoveParam_.adaptTime = 1.0f;
        railMoveParam_.timeEase.SetAdaptValue(&railMoveParam_.adaptTime);

        railMoveParam_.timeEase.SetMaxTime(directReturnParam_.maxTime);
        railMoveParam_.timeEase.SetType(static_cast<EasingType>(directReturnParam_.easeTypeInt));
        railMoveParam_.timeEase.SetIsStartEndReverse(true);
        railMoveParam_.timeEase.Reset();
    }
}

void RailData::UpdateReturn(float speedRate) {
    // 実際のデルタタイムを計算
    float actualDeltaTime = 0.0f;
    switch (static_cast<TimeMode>(timeModeSelector_.GetTimeModeInt())) {
    case TimeMode::DELTA_TIME:
        actualDeltaTime = Frame::DeltaTime() * speedRate;
        break;
    case TimeMode::DELTA_TIME_RATE:
        actualDeltaTime = Frame::DeltaTimeRate() * speedRate;
        break;
    default:
        break;
    }

    switch (railMoveParam_.returnMode) {
    case ReturnMode::REVERSE_RAIL:
        railMoveParam_.timeEase.Update(actualDeltaTime);

        // レール上の位置を計算
        currentPosition_ = startPosition_ + rail_->GetPositionOnRail(railMoveParam_.adaptTime);

        // 戻り完了チェック
        if (railMoveParam_.timeEase.IsFinished()) {
            railMoveParam_.adaptTime = 0.0f;
            currentPosition_         = startPosition_;
            OnReturnComplete();
        }
        break;

    case ReturnMode::DIRECT_RETURN:
        directReturnParam_.ease.Update(actualDeltaTime);
        currentPosition_ = directReturnParam_.easeAdaptPos;

        if (directReturnParam_.ease.IsFinished()) {
            currentPosition_ = startPosition_;
            OnReturnComplete();
        }
        break;

    default:
        break;
    }
}

void RailData::OnReturnComplete() {
    if (railMoveParam_.isLoop) {
        // ループする場合は再度再生開始
        playState_ = PlayState::PLAYING;

        // イージングをリセットして順方向に戻す
        railMoveParam_.adaptTime = 0.0f;
        railMoveParam_.timeEase.SetAdaptValue(&railMoveParam_.adaptTime);
        railMoveParam_.timeEase.SetStartValue(0.0f);
        railMoveParam_.timeEase.SetEndValue(1.0f);
        railMoveParam_.timeEase.SetMaxTime(railMoveParam_.maxTime);
        railMoveParam_.timeEase.SetType(static_cast<EasingType>(railMoveParam_.easeTypeInt));
        railMoveParam_.timeEase.SetIsStartEndReverse(false);
        railMoveParam_.timeEase.Reset();
    } else {
        // ループしない場合は停止
        playState_               = PlayState::STOPPED;
        railMoveParam_.adaptTime = 0.0f;

        // イージング設定を元に戻す
        railMoveParam_.timeEase.SetAdaptValue(&railMoveParam_.adaptTime);
        railMoveParam_.timeEase.SetStartValue(0.0f);
        railMoveParam_.timeEase.SetEndValue(1.0f);
        railMoveParam_.timeEase.SetMaxTime(railMoveParam_.maxTime);
        railMoveParam_.timeEase.SetType(static_cast<EasingType>(railMoveParam_.easeTypeInt));
        railMoveParam_.timeEase.SetIsStartEndReverse(false);
    }
}

void RailData::Play() {
    // 基底クラスのPlay()を呼ばずに独自実装
    Reset();
    startPosition_ = currentPosition_;
    playState_     = PlayState::PLAYING;
    railMoveParam_.timeEase.Reset();
}

void RailData::Reset() {
    railMoveParam_.elapsedTime = 0.0f;
    railMoveParam_.adaptTime   = 0.0f;
    currentPosition_           = startPosition_;
    railMoveParam_.timeEase.Reset();
    playState_ = PlayState::STOPPED;
}

void RailData::RegisterParams() {
    globalParameter_->Regist(groupName_, "maxTime", &railMoveParam_.maxTime);
    globalParameter_->Regist(groupName_, "startTime", &railMoveParam_.startTime);
    globalParameter_->Regist(groupName_, "isLoop", &railMoveParam_.isLoop);
    globalParameter_->Regist(groupName_, "easeType", &railMoveParam_.easeTypeInt);
    globalParameter_->Regist(groupName_, "returnMode", &railMoveParam_.returnModeInt);
    globalParameter_->Regist(groupName_, "returnTime", &directReturnParam_.maxTime);
    globalParameter_->Regist(groupName_, "returnEaseType", &directReturnParam_.easeTypeInt);
    globalParameter_->Regist(groupName_, "isLookAtDirection", &railMoveParam_.isLookAtDirection);
    timeModeSelector_.RegisterParam(groupName_, globalParameter_);
}

void RailData::GetParams() {
    railMoveParam_.maxTime           = globalParameter_->GetValue<float>(groupName_, "maxTime");
    railMoveParam_.startTime         = globalParameter_->GetValue<float>(groupName_, "startTime");
    railMoveParam_.isLoop            = globalParameter_->GetValue<bool>(groupName_, "isLoop");
    railMoveParam_.easeTypeInt       = globalParameter_->GetValue<int32_t>(groupName_, "easeType");
    railMoveParam_.returnModeInt     = globalParameter_->GetValue<int32_t>(groupName_, "returnMode");
    directReturnParam_.maxTime       = globalParameter_->GetValue<float>(groupName_, "returnTime");
    directReturnParam_.easeTypeInt   = globalParameter_->GetValue<int32_t>(groupName_, "returnEaseType");
    railMoveParam_.returnMode        = static_cast<ReturnMode>(railMoveParam_.returnModeInt);
    railMoveParam_.isLookAtDirection = globalParameter_->GetValue<bool>(groupName_, "isLookAtDirection");
    timeModeSelector_.GetParam(groupName_, globalParameter_);
}
void RailData::InitParams() {
    playState_                 = PlayState::STOPPED;
    currentPosition_           = Vector3::ZeroVector();
    startPosition_             = Vector3::ZeroVector();
    railMoveParam_.elapsedTime = 0.0f;
    railMoveParam_.adaptTime   = 0.0f;

    railMoveParam_.timeEase.SetAdaptValue(&railMoveParam_.adaptTime);
    railMoveParam_.timeEase.SetStartValue(0.0f);
    railMoveParam_.timeEase.SetEndValue(1.0f);
    railMoveParam_.timeEase.SetMaxTime(railMoveParam_.maxTime);
    railMoveParam_.timeEase.SetType(static_cast<EasingType>(railMoveParam_.easeTypeInt));
    railMoveParam_.timeEase.SetIsStartEndReverse(false);

    railMoveParam_.returnMode = static_cast<ReturnMode>(railMoveParam_.returnModeInt);
    railMoveParam_.timeEase.Reset();
}

void RailData::UpdateKeyFrameProgression() {
    // Railはキーフレーム進行を使用しない
}

void RailData::AdvanceToNexTSequenceElement() {
    // Railはキーフレーム進行を使用しない
}

std::unique_ptr<RailControlPoint> RailData::CreateKeyFrame(int32_t index) {
    auto keyFrame = std::make_unique<RailControlPoint>();
    keyFrame->Init(groupName_, index);
    return keyFrame;
}

std::string RailData::GeTSequenceElementFolderPath() const {
    return keyFramePath_ + groupName_ + "/";
}

void RailData::LoadSequenceElements() {
    BaseSequenceEffectData::LoadSequenceElements();
}

void RailData::SaveSequenceElements() {
    BaseSequenceEffectData::SaveSequenceElements();
}

void RailData::LoadData() {
    globalParameter_->LoadFile(groupName_, folderPath_);
    globalParameter_->SyncParamForGroup(groupName_);
    GetParams();
    LoadSequenceElements();
}

void RailData::SaveData() {
    globalParameter_->SaveFile(groupName_, folderPath_);
    SaveSequenceElements();
}

void RailData::AdjustParam() {
#ifdef _DEBUG
    if (showControls_) {
        ImGui::SeparatorText(("Rail Editor: " + groupName_).c_str());
        ImGui::PushID(groupName_.c_str());

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

        float progress = railMoveParam_.adaptTime;
        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f), "Progress");

        ImGui::Separator();
        ImGui::Checkbox("Look At Movement Direction", &railMoveParam_.isLookAtDirection);
        ImGui::Separator();

        ImGui::DragFloat("Start Time", &railMoveParam_.startTime, 0.01f, 0.0f, 10.0f);
        ImGui::DragFloat("Max Time", &railMoveParam_.maxTime, 0.01f, 0.1f, 10.0f);
        ImGui::Checkbox("Loop", &railMoveParam_.isLoop);

        ImGui::Separator();

        ImGuiEasingTypeSelector("Easing Type", railMoveParam_.easeTypeInt);

        railMoveParam_.timeEase.SetMaxTime(railMoveParam_.maxTime);
        railMoveParam_.timeEase.SetType(static_cast<EasingType>(railMoveParam_.easeTypeInt));

        ImGui::Separator();

        // タイムモード設定
        timeModeSelector_.SelectTimeModeImGui("Time Mode");

        ImGui::Separator();

        ImGui::SeparatorText("Return Settings");
        const char* returnModes[] = {"None", "Reverse Rail", "Direct Return"};
        ImGui::Combo("Return Mode", &railMoveParam_.returnModeInt, returnModes, IM_ARRAYSIZE(returnModes));

        if (playState_ != PlayState::RETURNING) {
            railMoveParam_.returnMode = static_cast<ReturnMode>(railMoveParam_.returnModeInt);
        }

        if (railMoveParam_.returnMode != ReturnMode::NONE) {
            ImGui::DragFloat("Return Time", &directReturnParam_.maxTime, 0.01f, 0.1f, 10.0f);
            ImGuiEasingTypeSelector("Return Easing Type", directReturnParam_.easeTypeInt);
        }

        ImGui::Separator();
      
        if (showControlPointLines_) {
            ImGui::SeparatorText("Control Points");

            if (ImGui::Button("Add Control Point")) {
                AddKeyFrame();
            }
            ImGui::SameLine();
            if (ImGui::Button("Clear All")) {
                ClearKeyFrames();
            }

            ImGui::Separator();

            for (int32_t i = 0; i < static_cast<int32_t>(sectionElements_.size()); ++i) {
                ImGui::PushID(i);
                sectionElements_[i]->AdjustParam();
                ImGui::PopID();
            }
        }

        ImGui::PopID();
    }
#endif
}

void RailData::SetControlPointLines(Line3D* line3d, const Vector4& color) {
    if (!line3d || !showControlPointLines_) {
        return;
    }

    if (sectionElements_.size() < 2) {
        return;
    }

    // 制御点の座標を取得
    std::vector<Vector3> positions;
    for (const auto& keyFrame : sectionElements_) {
        Vector3 pos = keyFrame->GetPosition() * direction_;

        if (parentTransform_ != nullptr) {
            Matrix4x4 parentMatrix = parentTransform_->matWorld_;
            Vector3 worldPos       = TransformMatrix(pos, parentMatrix);
            positions.push_back(worldPos);
        } else {
            positions.push_back(pos);
        }
    }

    // Catmull-Rom曲線に沿ってラインを描画
    const size_t lineSegments = 20; // 各区間の分割数
    for (size_t i = 0; i < positions.size() - 1; ++i) {
        for (size_t j = 0; j < lineSegments; ++j) {
            float t1 = static_cast<float>(j) / static_cast<float>(lineSegments);
            float t2 = static_cast<float>(j + 1) / static_cast<float>(lineSegments);

            // 全体の進行度を計算
            float globalT1 = (static_cast<float>(i) + t1) / static_cast<float>(positions.size() - 1);
            float globalT2 = (static_cast<float>(i) + t2) / static_cast<float>(positions.size() - 1);

            Vector3 start = startPosition_ + CatmullRomPosition(positions, globalT1);
            Vector3 end   = startPosition_ + CatmullRomPosition(positions, globalT2);

            line3d->SetLine(start, end, color);
        }
    }
}
Vector3 RailData::GetMovementDirection() const {
    Vector3 direction = currentPosition_ - railMoveParam_.previousPosition;

    if (direction.Length() < 0.001f) {
        return Vector3::ToForward(); 
    }

    return direction.Normalize();
}
