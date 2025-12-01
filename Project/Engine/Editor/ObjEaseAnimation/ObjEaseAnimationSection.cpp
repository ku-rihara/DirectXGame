#include "ObjEaseAnimationSection.h"
#include "Frame/Frame.h"
#include "MathFunction.h"
#include <imgui.h>

void ObjEaseAnimationSection::Init(const std::string& animationName, const std::string& categoryName, const int32_t& keyNumber) {
    globalParameter_             = GlobalParameter::GetInstance();
    currenTSequenceElementIndex_ = keyNumber;
    groupName_                   = animationName + std::to_string(currenTSequenceElementIndex_);
    folderPath_                  = "ObjEaseAnimation/" + categoryName + "/" + "Sections/" + animationName + "/";

    // Scaleの初期値を1に設定
    transformParams_[static_cast<size_t>(TransformType::Scale)].endValue = Vector3::OneVector();

    // Rail初期化
    railPlayer_ = std::make_unique<RailPlayer>();
    railPlayer_->Init();

    // パラメータ登録・取得
    if (!globalParameter_->HasRegisters(groupName_)) {
        globalParameter_->CreateGroup(groupName_);
        RegisterParams();
    } else {
        GetParams();
    }

    AdaptValueSetting();
    AdaptEaseParam();
}

void ObjEaseAnimationSection::Reset() {
    // 全Transformのリセット
    for (auto& param : transformParams_) {
        param.ease.Reset();
        param.returnEase.Reset();
        param.currentOffset     = param.startValue;
        param.state             = param.isActive ? TransformState::INACTIVE : TransformState::INACTIVE;
        param.returnElapsedTime = 0.0f;
    }

    // レールを停止
    if (railPlayer_) {
        railPlayer_->Stop();
    }

    // 再生状態を停止に
    playState_ = PlayState::STOPPED;

    // 経過時間リセット
    elapsedTime_ = 0.0f;

    // イージング再適応
    AdaptValueSetting();
    AdaptEaseParam();
}

void ObjEaseAnimationSection::LoadData() {
    // パラメータ読み込み・同期
    globalParameter_->LoadFile(groupName_, folderPath_);
    globalParameter_->SyncParamForGroup(groupName_);
}

void ObjEaseAnimationSection::SaveData() {
    // セーブ
    globalParameter_->SaveFile(groupName_, folderPath_);
}

void ObjEaseAnimationSection::Update(const float& speedRate) {
    float actualDeltaTime;

    // 時間モードに応じてデルタタイム取得
    switch (static_cast<TimeMode>(timeModeSelector_.GetTimeModeInt())) {
    case TimeMode::DELTA_TIME:
        actualDeltaTime = Frame::DeltaTime() * speedRate;
        break;
    case TimeMode::DELTA_TIME_RATE:
    default:
        actualDeltaTime = Frame::DeltaTimeRate() * speedRate;
        break;
    }

    // 停止中は何もしない
    if (playState_ == PlayState::STOPPED) {
        return;
    }

    // 待機中の場合、経過時間を加算してstartTime_チェック
    if (playState_ == PlayState::WAITING) {
        elapsedTime_ += actualDeltaTime;

        // startTime_を超えたら再生開始
        if (elapsedTime_ >= startTime_) {
            StartPlay();
        }
        return;
    }

    // 各Transformを個別に更新
    for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
        auto& param = transformParams_[i];
        if (param.isActive) {
            UpdateTransform(param, static_cast<TransformType>(i), actualDeltaTime);
        }
    }

    // 全体の状態を更新
    UpdateOverallState();
}

void ObjEaseAnimationSection::UpdateTransform(TransformParam& param, TransformType type, const float& deltaTime) {
    switch (param.state) {
    case TransformState::INACTIVE:
        // 何もしない
        break;

    case TransformState::WAITING:
        // 待機状態(StartPlayで遷移)
        break;

    case TransformState::PLAYING:
        UpdateTransformPlay(param, type, deltaTime);
        break;

    case TransformState::RETURN_WAITING:
        // 戻り待機中
        param.returnElapsedTime += deltaTime;
        if (param.returnElapsedTime >= returnStartTime_) {
            // 戻り開始
            if (type == TransformType::Translation && param.useRail) {
                // Rail使用時は戻り処理をスキップして完了
                param.state = TransformState::FINISHED;
            } else {
                // 戻り用イージングの設定
                param.returnEase.SetAdaptValue(&param.currentOffset);
                param.returnEase.SetStartValue(param.currentOffset);
                param.returnEase.SetEndValue(param.startValue);
                param.returnEase.SetMaxTime(param.returnMaxTime);
                param.returnEase.SetType(static_cast<EasingType>(param.returnEaseType));
                param.returnEase.SetIsStartEndReverse(false);
                param.returnEase.Reset();
                param.state = TransformState::RETURNING;
            }
        }
        break;

    case TransformState::RETURNING:
        UpdateTransformReturn(param, type, deltaTime);
        break;

    case TransformState::FINISHED:
        // 完了
        break;
    }
}

void ObjEaseAnimationSection::UpdateTransformPlay(TransformParam& param, TransformType type, const float& deltaTime) {
    bool isFinished = false;

    // レールの更新かイージング更新で分岐
    if (type == TransformType::Translation && param.useRail) {
        railPlayer_->Update();
        param.currentOffset = railPlayer_->GetCurrentPosition();
        isFinished          = !railPlayer_->IsPlaying() && !railPlayer_->IsReturning();
    } else {
        param.ease.Update(deltaTime);
        param.currentOffset = param.ease.GetValue();
        isFinished          = param.ease.IsFinished();
    }

    // 再生が完了したら次の状態へ
    if (isFinished) {
        if (param.isReturnToOrigin) {
            // 戻り待機状態に遷移
            param.state             = TransformState::RETURN_WAITING;
            param.returnElapsedTime = 0.0f;
        } else {
            // 戻りが不要なら完了
            param.state = TransformState::FINISHED;
        }
    }
}

void ObjEaseAnimationSection::UpdateTransformReturn(TransformParam& param, TransformType type, const float& deltaTime) {
    // Rail使用時はスキップ(RETURN_WAITINGで処理済み)
    if (type == TransformType::Translation && param.useRail) {
        param.state = TransformState::FINISHED;
        return;
    }

    param.returnEase.Update(deltaTime);
    param.currentOffset = param.returnEase.GetValue();

    if (param.returnEase.IsFinished()) {
        param.state = TransformState::FINISHED;
    }
}

void ObjEaseAnimationSection::UpdateOverallState() {
    // 全てのTransformが完了したかチェック
    if (AreAllTransformsFinished()) {
        playState_ = PlayState::STOPPED;
    } else {
        // まだ再生中のTransformがあれば再生状態を維持
        bool anyPlaying   = false;
        bool anyReturning = false;

        for (const auto& param : transformParams_) {
            if (!param.isActive)
                continue;

            if (param.state == TransformState::PLAYING) {
                anyPlaying = true;
            } else if (param.state == TransformState::RETURNING || param.state == TransformState::RETURN_WAITING) {
                anyReturning = true;
            }
        }

        if (anyReturning) {
            playState_ = PlayState::RETURNING;
        } else if (anyPlaying) {
            playState_ = PlayState::PLAYING;
        }
    }
}

bool ObjEaseAnimationSection::AreAllTransformsFinished() const {
    for (const auto& param : transformParams_) {
        if (!param.isActive)
            continue;

        if (param.state != TransformState::FINISHED) {
            return false;
        }
    }
    return true;
}

void ObjEaseAnimationSection::StartPlay() {
    playState_ = PlayState::PLAYING;

    // 全てのイージングをリセットして開始
    for (auto& param : transformParams_) {
        if (param.isActive) {
            param.ease.Reset();
            param.state             = TransformState::PLAYING;
            param.returnElapsedTime = 0.0f;
        }
    }

    // Rail使用時はRailを再生
    const auto& transParam = transformParams_[static_cast<size_t>(TransformType::Translation)];
    if (transParam.isActive && transParam.useRail && !transParam.railFileName.empty()) {
        railPlayer_->Play(transParam.railFileName);
    }
}

void ObjEaseAnimationSection::RegisterParams() {
    globalParameter_->Regist(groupName_, "startTime", &startTime_);
    globalParameter_->Regist(groupName_, "returnStartTime", &returnStartTime_);
    timeModeSelector_.RegisterParam(groupName_, globalParameter_);

    for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
        const char* name = GetSRTName(static_cast<TransformType>(i));
        auto& param      = transformParams_[i];

        globalParameter_->Regist(groupName_, std::string(name) + "_IsActive", &param.isActive);
        globalParameter_->Regist(groupName_, std::string(name) + "_IsReturnToOrigin", &param.isReturnToOrigin);
        globalParameter_->Regist(groupName_, std::string(name) + "_endValue", &param.endValue);
        globalParameter_->Regist(groupName_, std::string(name) + "_MaxTime", &param.maxTime);
        globalParameter_->Regist(groupName_, std::string(name) + "_EaseType", &param.easeType);
        globalParameter_->Regist(groupName_, std::string(name) + "_ReturnMaxTime", &param.returnMaxTime);
        globalParameter_->Regist(groupName_, std::string(name) + "_ReturnEaseType", &param.returnEaseType);

        if (i == static_cast<size_t>(TransformType::Translation)) {
            globalParameter_->Regist(groupName_, std::string(name) + "_UseRail", &param.useRail);
            globalParameter_->Regist(groupName_, std::string(name) + "_RailFileName", &param.railFileName);
        }
    }
}

void ObjEaseAnimationSection::GetParams() {
    startTime_ = globalParameter_->GetValue<float>(groupName_, "startTime");
    timeModeSelector_.GetParam(groupName_, globalParameter_);
    returnStartTime_ = globalParameter_->GetValue<float>(groupName_, "returnStartTime");

    for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
        const char* name = GetSRTName(static_cast<TransformType>(i));
        auto& param      = transformParams_[i];

        param.isActive         = globalParameter_->GetValue<bool>(groupName_, std::string(name) + "_IsActive");
        param.isReturnToOrigin = globalParameter_->GetValue<bool>(groupName_, std::string(name) + "_IsReturnToOrigin");
        param.endValue         = globalParameter_->GetValue<Vector3>(groupName_, std::string(name) + "_endValue");
        param.maxTime          = globalParameter_->GetValue<float>(groupName_, std::string(name) + "_MaxTime");
        param.easeType         = globalParameter_->GetValue<int32_t>(groupName_, std::string(name) + "_EaseType");
        param.returnMaxTime    = globalParameter_->GetValue<float>(groupName_, std::string(name) + "_ReturnMaxTime");
        param.returnEaseType   = globalParameter_->GetValue<int32_t>(groupName_, std::string(name) + "_ReturnEaseType");

        if (i == static_cast<size_t>(TransformType::Translation)) {
            param.useRail      = globalParameter_->GetValue<bool>(groupName_, std::string(name) + "_UseRail");
            param.railFileName = globalParameter_->GetValue<std::string>(groupName_, std::string(name) + "_RailFileName");
        }
    }
}

void ObjEaseAnimationSection::AdaptEaseParam() {
    for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
        auto& param = transformParams_[i];

        if (i == static_cast<size_t>(TransformType::Translation) && param.useRail) {
            continue;
        }

        // 通常イージング設定
        param.ease.SetMaxTime(param.maxTime);
        param.ease.SetEndValue(param.endValue);
        param.ease.SetType(static_cast<EasingType>(param.easeType));
        param.ease.SetIsStartEndReverse(false);
    }
}

void ObjEaseAnimationSection::ImGuiTransformParam(const char* label, TransformParam& param, const TransformType& type) {
    ImGui::SeparatorText(label);
    ImGui::Checkbox((std::string(label) + " Active").c_str(), &param.isActive);
    ImGui::PushID(label);

    if (!param.isActive) {
        ImGui::PopID();
        return;
    }

    // 戻りフラグ
    ImGui::Checkbox((std::string(label) + " Return To Origin").c_str(), &param.isReturnToOrigin);

    if (type == TransformType::Translation) {
        ImGui::Checkbox("Use Rail", &param.useRail);

        if (param.useRail) {
            std::string directory = globalParameter_->GetDirectoryPath() + "RailEditor/Dates";
            railFileSelector_.selector.SelectFile("Rail File", directory, param.railFileName, "", true);
            ImGui::PopID();
            return;
        }
    }

    if (type == TransformType::Rotation) {
        Vector3 endValueDeg = ToDegree(param.endValue);
        ImGui::DragFloat3("End Value (Degree)", &endValueDeg.x, 0.1f);
        param.endValue = ToRadian(endValueDeg);
    } else {
        ImGui::DragFloat3("End Value", &param.endValue.x, 0.01f);
    }

    ImGui::DragFloat("Max Time", &param.maxTime, 0.01f, 0.1f, 10.0f);
    ImGuiEasingTypeSelector("Easing Type", param.easeType);

    // 戻り設定
    if (param.isReturnToOrigin) {
        ImGui::Separator();
        ImGui::Text("Return Settings:");
        ImGui::DragFloat("Return Start Time", &returnStartTime_, 0.01f, 0.0f, 100.0f);
        ImGui::DragFloat("Return Max Time", &param.returnMaxTime, 0.01f, 0.0f, 10.0f);
        ImGuiEasingTypeSelector("Return Easing Type", param.returnEaseType);
    }

    ImGui::PopID();
}

void ObjEaseAnimationSection::AdjustParam() {
#ifdef _DEBUG
    ImGui::SeparatorText(("Section: " + groupName_).c_str());
    ImGui::PushID(groupName_.c_str());

    ImGui::DragFloat("Start Time", &startTime_, 0.01f, 0.0f, 100.0f);
    timeModeSelector_.SelectTimeModeImGui("Time Mode");

    ImGui::Separator();

    for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
        ImGuiTransformParam(GetSRTName(static_cast<TransformType>(i)), transformParams_[i], static_cast<TransformType>(i));
    }

    AdaptEaseParam();

    ImGui::PopID();
#endif
}

void ObjEaseAnimationSection::AdaptValueSetting() {
    for (auto& param : transformParams_) {
        param.ease.SetAdaptValue(&param.currentOffset);
    }
}

void ObjEaseAnimationSection::SetStartValues(const Vector3& scale, const Vector3& rotation, const Vector3& translation) {
    transformParams_[static_cast<size_t>(TransformType::Scale)].ease.SetStartValue(scale);
    transformParams_[static_cast<size_t>(TransformType::Rotation)].ease.SetStartValue(rotation);
    transformParams_[static_cast<size_t>(TransformType::Translation)].ease.SetStartValue(translation);

    transformParams_[static_cast<size_t>(TransformType::Scale)].startValue       = scale;
    transformParams_[static_cast<size_t>(TransformType::Rotation)].startValue    = rotation;
    transformParams_[static_cast<size_t>(TransformType::Translation)].startValue = translation;

    transformParams_[static_cast<size_t>(TransformType::Scale)].currentOffset       = scale;
    transformParams_[static_cast<size_t>(TransformType::Rotation)].currentOffset    = rotation;
    transformParams_[static_cast<size_t>(TransformType::Translation)].currentOffset = translation;
}

bool ObjEaseAnimationSection::IsFinished() const {
    return AreAllTransformsFinished();
}

bool ObjEaseAnimationSection::IsUsingRail() const {
    const auto& transParam = transformParams_[static_cast<size_t>(TransformType::Translation)];
    return transParam.useRail && transParam.isActive;
}

const char* ObjEaseAnimationSection::GetSRTName(const TransformType& type) const {
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

void ObjEaseAnimationSection::StartWaiting() {
    playState_   = PlayState::WAITING;
    elapsedTime_ = 0.0f;

    // 各Transformを待機状態に設定
    for (auto& param : transformParams_) {
        if (param.isActive) {
            param.state             = TransformState::WAITING;
            param.returnElapsedTime = 0.0f;
        }
    }
}