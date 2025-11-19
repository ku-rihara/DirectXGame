#include "ObjEaseAnimationData.h"
#include "3d/WorldTransform.h"
#include "MathFunction.h"
#include <algorithm>
#include <imgui.h>

void ObjEaseAnimationData::Init(const std::string& animationName, const std::string& categoryName) {
    globalParameter_ = GlobalParameter::GetInstance();
    groupName_       = animationName;
    categoryName_    = categoryName;
    folderPath_      = "ObjEaseAnimation/" + categoryName_;
    transformParams_[static_cast<size_t>(TransformType::Scale)].startValue = Vector3::OneVector();

    // Rail初期化
    railPlayer_ = std::make_unique<RailPlayer>();
    railPlayer_->Init();

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

void ObjEaseAnimationData::Update(const float& deltaTime) {
    if (playState_ == PlayState::RETURNING) {
        UpdateReturn(deltaTime);
        return;
    }

    if (playState_ != PlayState::PLAYING) {
        return;
    }

    UpdateTransforms(deltaTime);
    CheckFinish();
}

void ObjEaseAnimationData::UpdateTransforms(const float& deltaTime) {
    for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
        auto& param = transformParams_[i];

        if (!param.isActive) {
            if (i == static_cast<size_t>(TransformType::Scale)) {
                param.currentOffset = Vector3::OneVector();
            }
            continue;
        }

        if (i == static_cast<size_t>(TransformType::Translation) && param.useRail) {
            railPlayer_->Update(deltaTime);
            param.currentOffset = railPlayer_->GetCurrentPosition();
        } else {
            param.ease.Update(deltaTime);
            param.currentOffset = param.ease.GetValue();
        }
    }
}

void ObjEaseAnimationData::UpdateReturn(const float& deltaTime) {
    bool allFinished = true;

    for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
        auto& param = transformParams_[i];

        if (!param.isActive || !param.returnToOrigin)
            continue;

        // Translation + Rail使用時の特殊処理
        if (i == static_cast<size_t>(TransformType::Translation) && param.useRail) {
            railPlayer_->Update(deltaTime);
            param.currentOffset = originalValues_[i] + railPlayer_->GetCurrentPosition();
            if (railPlayer_->IsPlaying()) {
                allFinished = false;
            }
        } else {
            param.ease.Update(deltaTime);
            param.currentOffset = param.ease.GetValue();
            if (!param.ease.IsFinished()) {
                allFinished = false;
            }
        }
    }

    if (allFinished) {
        playState_ = PlayState::STOPPED;
    }
}

void ObjEaseAnimationData::CheckFinish() {
    bool allFinished = true;

    for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
        const auto& param = transformParams_[i];

        if (!param.isActive)
            continue;

        // Translation + Rail使用時の特殊処理
        if (i == static_cast<size_t>(TransformType::Translation) && param.useRail) {
            if (railPlayer_->IsPlaying()) {
                allFinished = false;
            }
        } else {
            if (!param.ease.IsFinished()) {
                allFinished = false;
            }
        }
    }

    if (allFinished) {
        // 戻り動作が必要かチェック
        bool needReturn = false;
        for (const auto& param : transformParams_) {
            if (param.isActive && param.returnToOrigin) {
                needReturn = true;
                break;
            }
        }

        if (needReturn) {
            StartReturn();
        } else {
            playState_ = PlayState::STOPPED;
        }
    }
}

void ObjEaseAnimationData::StartReturn() {
    playState_ = PlayState::RETURNING;

    for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
        auto& param = transformParams_[i];

        if (!param.isActive || !param.returnToOrigin)
            continue;

        if (i == static_cast<size_t>(TransformType::Translation) && param.useRail) {
            continue;
        }

        // 戻り設定
        param.ease.SetIsStartEndReverse(true);
        param.ease.SetMaxTime(param.returnMaxTime);
        param.ease.SetType(static_cast<EasingType>(param.returnEaseType));
        param.ease.Reset();
    }
}

void ObjEaseAnimationData::Play() {
    Reset();
    playState_ = PlayState::PLAYING;

    for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
        auto& param = transformParams_[i];

        if (!param.isActive)
            continue;

        // Translation + Rail使用時の特殊処理
        if (i == static_cast<size_t>(TransformType::Translation) && param.useRail && !param.railFileName.empty()) {
            railPlayer_->Play(param.railFileName);
            continue;
        }

        // 通常のイージング設定
        param.ease.SetAdaptValue(&param.currentOffset);
        param.ease.SetStartValue(param.startValue);
        param.ease.SetEndValue(param.endValue);
        param.ease.SetMaxTime(param.maxTime);
        param.ease.SetType(static_cast<EasingType>(param.easeType));
        param.ease.SetIsStartEndReverse(false); 
        param.ease.Reset();
    }
}

void ObjEaseAnimationData::Stop() {
    playState_ = PlayState::STOPPED;

    // 現在値を初期値に戻す
    for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
        transformParams_[i].currentOffset = originalValues_[i];
    }

    if (railPlayer_) {
        railPlayer_->Stop();
    }
}

void ObjEaseAnimationData::Reset() {
    for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
        transformParams_[i].currentOffset = transformParams_[i].startValue;
        transformParams_[i].ease.Reset();
    }

    if (railPlayer_) {
        railPlayer_->Stop();
    }
}

bool ObjEaseAnimationData::IsFinished() const {
    return playState_ == PlayState::STOPPED;
}

bool ObjEaseAnimationData::IsPlaying() const {
    return playState_ == PlayState::PLAYING || playState_ == PlayState::RETURNING;
}

void ObjEaseAnimationData::LoadData() {
    globalParameter_->LoadFile(groupName_, folderPath_);
    globalParameter_->SyncParamForGroup(groupName_);
    GetParams();
}

void ObjEaseAnimationData::SaveData() {
    globalParameter_->SaveFile(groupName_, folderPath_);
}

void ObjEaseAnimationData::RegisterParams() {
    for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
        const char* name = GetSRTName(static_cast<TransformType>(i));
        auto& param      = transformParams_[i];

        globalParameter_->Regist(groupName_, std::string(name) + "_IsActive", &param.isActive);
        // useCurrentAsStart を削除
        globalParameter_->Regist(groupName_, std::string(name) + "_ReturnToOrigin", &param.returnToOrigin);
        globalParameter_->Regist(groupName_, std::string(name) + "_startValue", &param.startValue);
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

void ObjEaseAnimationData::GetParams() {
    for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
        const char* name = GetSRTName(static_cast<TransformType>(i));
        auto& param      = transformParams_[i];

        param.isActive       = globalParameter_->GetValue<bool>(groupName_, std::string(name) + "_IsActive");
        param.returnToOrigin = globalParameter_->GetValue<bool>(groupName_, std::string(name) + "_ReturnToOrigin");
        param.startValue     = globalParameter_->GetValue<Vector3>(groupName_, std::string(name) + "_startValue");
        param.endValue       = globalParameter_->GetValue<Vector3>(groupName_, std::string(name) + "_endValue");
        param.maxTime        = globalParameter_->GetValue<float>(groupName_, std::string(name) + "_MaxTime");
        param.easeType       = globalParameter_->GetValue<int32_t>(groupName_, std::string(name) + "_EaseType");
        param.returnMaxTime  = globalParameter_->GetValue<float>(groupName_, std::string(name) + "_ReturnMaxTime");
        param.returnEaseType = globalParameter_->GetValue<int32_t>(groupName_, std::string(name) + "_ReturnEaseType");

        if (i == static_cast<size_t>(TransformType::Translation)) {
            param.useRail      = globalParameter_->GetValue<bool>(groupName_, std::string(name) + "_UseRail");
            param.railFileName = globalParameter_->GetValue<std::string>(groupName_, std::string(name) + "_RailFileName");
        }
    }
}

void ObjEaseAnimationData::ResetParams() {
    playState_ = PlayState::STOPPED;

    for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
        transformParams_[i].currentOffset = transformParams_[i].startValue;
        originalValues_[i]                = transformParams_[i].startValue;
    }
}

void ObjEaseAnimationData::ImGuiTransformParam(const char* label, TransformParam& param, const TransformType& TransformTypeType) {
    ImGui::SeparatorText(label);
    ImGui::Checkbox((std::string(label) + " Active").c_str(), &param.isActive);
    ImGui::PushID(label);

    if (!param.isActive) {
        ImGui::PopID();
        return;
    }

    ImGui::Checkbox("Return To Origin", &param.returnToOrigin);

    // Rail使用フラグ
    if (TransformTypeType == TransformType::Translation) {
        ImGui::Checkbox("Use Rail", &param.useRail);

        if (param.useRail) {
            // Railファイル選択
            std::string directory = "Resources/GlobalParameter/RailEditor/Dates";
            railFileSelector_.selector.SelectFile("Rail File", directory, param.railFileName, "", true);
            return; 
        }
    }

    if (TransformTypeType == TransformType::Rotation) {
        // Radian から Degree に変換して表示
        Vector3 startValueDeg = ToDegree(param.startValue);
        Vector3 endValueDeg   = ToDegree(param.endValue);

        ImGui::DragFloat3("Start Value (Degree)", &startValueDeg.x, 0.1f);
        ImGui::DragFloat3("End Value (Degree)", &endValueDeg.x, 0.1f);

        // Degree から Radian に変換して保存
        param.startValue = ToRadian(startValueDeg);
        param.endValue   = ToRadian(endValueDeg);
    } else {

        ImGui::DragFloat3("Start Value", &param.startValue.x, 0.01f);
        ImGui::DragFloat3("End Value", &param.endValue.x, 0.01f);
    }
    // maxTime, easeType
    ImGui::DragFloat("Max Time", &param.maxTime, 0.01f, 0.1f, 10.0f);
    ImGuiEasingTypeSelector("Easing Type", param.easeType);

    // Return Parameters
    if (param.returnToOrigin) {
        ImGui::SeparatorText("Return Parameters");
        ImGui::DragFloat("Return Max Time", &param.returnMaxTime, 0.01f, 0.1f, 10.0f);
        ImGuiEasingTypeSelector("Return Easing Type", param.returnEaseType);
    }
    ImGui::PopID();
}

void ObjEaseAnimationData::AdjustParam() {
#ifdef _DEBUG
    if (showControls_) {
        ImGui::SeparatorText(("Animation: " + groupName_).c_str());
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
        case PlayState::RETURNING:
            stateText = "RETURNING";
            break;
        }
        ImGui::Text("State: %s", stateText);

        ImGui::Separator();

        // 各Transformパラメータ
        for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
            ImGuiTransformParam(GetSRTName(static_cast<TransformType>(i)), transformParams_[i], static_cast<TransformType>(i));
        }

        ImGui::PopID();
    }
#endif // _DEBUG
}

//*-------------------------------getter Method-------------------------------*/

bool ObjEaseAnimationData::IsUsingRail() const {
    const auto& transParam = transformParams_[static_cast<size_t>(TransformType::Translation)];
    return transParam.useRail && transParam.isActive;
}

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
