#include "ObjEaseAnimationData.h"
#include "MathFunction.h"
#include <algorithm>
#include <imgui.h>

void ObjEaseAnimationData::Init(const std::string& animationName, const std::string& categoryName) {
    globalParameter_ = GlobalParameter::GetInstance();
    groupName_       = animationName;
    categoryName_    = categoryName;
    folderPath_      = "ObjEaseAnimation/" + categoryName_;

    // Rail初期化
    railPlayer_ = std::make_unique<RailPlayer>();
    railPlayer_->Init();

    if (!globalParameter_->HasRegisters(groupName_)) {
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
    // Scale更新
    if (scaleParam_.isActive) {
        scaleParam_.ease.Update(deltaTime);
        scaleParam_.currentValue = scaleParam_.ease.GetValue();
    }

    // Rotation更新
    if (rotationParam_.isActive) {
        rotationParam_.ease.Update(deltaTime);
        rotationParam_.currentValue = rotationParam_.ease.GetValue();
    }

    // Translation更新
    if (translationParam_.isActive) {
        if (translationParam_.useRail) {
            // Rail使用
            railPlayer_->Update(deltaTime);
            translationParam_.currentValue = originalTranslation_ + railPlayer_->GetCurrentPosition();
        } else {
            // 通常のイージング
            translationParam_.ease.Update(deltaTime);
            translationParam_.currentValue = translationParam_.ease.GetValue();
        }
    }
}

void ObjEaseAnimationData::UpdateReturn(const float& deltaTime) {
    bool allFinished = true;

    // Scale戻り
    if (scaleParam_.isActive && scaleParam_.returnToOrigin) {
        scaleParam_.ease.Update(deltaTime);
        scaleParam_.currentValue = scaleParam_.ease.GetValue();
        if (!scaleParam_.ease.IsFinished()) {
            allFinished = false;
        }
    }

    // Rotation戻り
    if (rotationParam_.isActive && rotationParam_.returnToOrigin) {
        rotationParam_.ease.Update(deltaTime);
        rotationParam_.currentValue = rotationParam_.ease.GetValue();
        if (!rotationParam_.ease.IsFinished()) {
            allFinished = false;
        }
    }

    // Translation戻り
    if (translationParam_.isActive && translationParam_.returnToOrigin) {
        if (translationParam_.useRail) {
            railPlayer_->Update(deltaTime);
            translationParam_.currentValue = originalTranslation_ + railPlayer_->GetCurrentPosition();
            if (railPlayer_->IsPlaying()) {
                allFinished = false;
            }
        } else {
            translationParam_.ease.Update(deltaTime);
            translationParam_.currentValue = translationParam_.ease.GetValue();
            if (!translationParam_.ease.IsFinished()) {
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

    // 各Transformの完了をチェック
    if (scaleParam_.isActive && !scaleParam_.ease.IsFinished()) {
        allFinished = false;
    }
    if (rotationParam_.isActive && !rotationParam_.ease.IsFinished()) {
        allFinished = false;
    }
    if (translationParam_.isActive) {
        if (translationParam_.useRail) {
            if (railPlayer_->IsPlaying()) {
                allFinished = false;
            }
        } else {
            if (!translationParam_.ease.IsFinished()) {
                allFinished = false;
            }
        }
    }

    if (allFinished) {
        // 戻り動作が必要かチェック
        bool needReturn = false;
        if (scaleParam_.isActive && scaleParam_.returnToOrigin)
            needReturn = true;
        if (rotationParam_.isActive && rotationParam_.returnToOrigin)
            needReturn = true;
        if (translationParam_.isActive && translationParam_.returnToOrigin)
            needReturn = true;

        if (needReturn) {
            StartReturn();
        } else {
            playState_ = PlayState::STOPPED;
        }
    }
}

void ObjEaseAnimationData::StartReturn() {
    playState_ = PlayState::RETURNING;

    // Scale戻り設定（StartとEndを入れ替え）
    if (scaleParam_.isActive && scaleParam_.returnToOrigin) {
        scaleParam_.ease.SetStartValue(scaleParam_.endValue);
        scaleParam_.ease.SetEndValue(scaleParam_.startValue);
        scaleParam_.ease.SetMaxTime(scaleParam_.returnMaxTime);
        scaleParam_.ease.SetType(static_cast<EasingType>(scaleParam_.returnEaseType));
        scaleParam_.ease.Reset();
    }

    // Rotation戻り設定（StartとEndを入れ替え）
    if (rotationParam_.isActive && rotationParam_.returnToOrigin) {
        rotationParam_.ease.SetStartValue(rotationParam_.endValue);
        rotationParam_.ease.SetEndValue(rotationParam_.startValue);
        rotationParam_.ease.SetMaxTime(rotationParam_.returnMaxTime);
        rotationParam_.ease.SetType(static_cast<EasingType>(rotationParam_.returnEaseType));
        rotationParam_.ease.Reset();
    }

    // Translation戻り設定（StartとEndを入れ替え）
    if (translationParam_.isActive && translationParam_.returnToOrigin) {
        if (translationParam_.useRail) {
            // Railの戻り処理はRailPlayer側で処理される
            // 特に何もしない（Railが自動で戻る設定になっている）
        } else {
            translationParam_.ease.SetStartValue(translationParam_.endValue);
            translationParam_.ease.SetEndValue(translationParam_.startValue);
            translationParam_.ease.SetMaxTime(translationParam_.returnMaxTime);
            translationParam_.ease.SetType(static_cast<EasingType>(translationParam_.returnEaseType));
            translationParam_.ease.Reset();
        }
    }
}

void ObjEaseAnimationData::Play() {
    Reset();
    playState_ = PlayState::PLAYING;

    // Scale設定
    if (scaleParam_.isActive) {
        scaleParam_.ease.SetAdaptValue(&scaleParam_.currentValue);
        scaleParam_.ease.SetStartValue(scaleParam_.startValue);
        scaleParam_.ease.SetEndValue(scaleParam_.endValue);
        scaleParam_.ease.SetMaxTime(scaleParam_.maxTime);
        scaleParam_.ease.SetType(static_cast<EasingType>(scaleParam_.easeType));
        scaleParam_.ease.Reset();
    }

    // Rotation設定
    if (rotationParam_.isActive) {
        rotationParam_.ease.SetAdaptValue(&rotationParam_.currentValue);
        rotationParam_.ease.SetStartValue(rotationParam_.startValue);
        rotationParam_.ease.SetEndValue(rotationParam_.endValue);
        rotationParam_.ease.SetMaxTime(rotationParam_.maxTime);
        rotationParam_.ease.SetType(static_cast<EasingType>(rotationParam_.easeType));
        rotationParam_.ease.Reset();
    }

    // Translation設定
    if (translationParam_.isActive) {
        if (translationParam_.useRail && !translationParam_.railFileName.empty()) {
            // Rail再生
            railPlayer_->Play(translationParam_.railFileName);
        } else {
            translationParam_.ease.SetAdaptValue(&translationParam_.currentValue);
            translationParam_.ease.SetStartValue(translationParam_.startValue);
            translationParam_.ease.SetEndValue(translationParam_.endValue);
            translationParam_.ease.SetMaxTime(translationParam_.maxTime);
            translationParam_.ease.SetType(static_cast<EasingType>(translationParam_.easeType));
            translationParam_.ease.Reset();
        }
    }
}

void ObjEaseAnimationData::Stop() {
    playState_ = PlayState::STOPPED;

    // 現在値を初期値に戻す
    scaleParam_.currentValue       = originalScale_;
    rotationParam_.currentValue    = originalRotation_;
    translationParam_.currentValue = originalTranslation_;

    if (railPlayer_) {
        railPlayer_->Stop();
    }
}

void ObjEaseAnimationData::Reset() {
    scaleParam_.currentValue       = scaleParam_.startValue;
    rotationParam_.currentValue    = rotationParam_.startValue;
    translationParam_.currentValue = translationParam_.startValue;

    scaleParam_.ease.Reset();
    rotationParam_.ease.Reset();
    translationParam_.ease.Reset();

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
    LoadParams();
}

void ObjEaseAnimationData::SaveData() {
    globalParameter_->SaveFile(groupName_, folderPath_);
}

void ObjEaseAnimationData::RegisterParams() {
    // Scale
    globalParameter_->Regist(groupName_, "Scale_IsActive", &scaleParam_.isActive);
    globalParameter_->Regist(groupName_, "Scale_ReturnToOrigin", &scaleParam_.returnToOrigin);
    globalParameter_->Regist(groupName_, "Scale_StartValue", &scaleParam_.startValue);
    globalParameter_->Regist(groupName_, "Scale_EndValue", &scaleParam_.endValue);
    globalParameter_->Regist(groupName_, "Scale_MaxTime", &scaleParam_.maxTime);
    globalParameter_->Regist(groupName_, "Scale_EaseType", &scaleParam_.easeType);
    globalParameter_->Regist(groupName_, "Scale_ReturnMaxTime", &scaleParam_.returnMaxTime);
    globalParameter_->Regist(groupName_, "Scale_ReturnEaseType", &scaleParam_.returnEaseType);

    // Rotation
    globalParameter_->Regist(groupName_, "Rotation_IsActive", &rotationParam_.isActive);
    globalParameter_->Regist(groupName_, "Rotation_ReturnToOrigin", &rotationParam_.returnToOrigin);
    globalParameter_->Regist(groupName_, "Rotation_StartValue", &rotationParam_.startValue);
    globalParameter_->Regist(groupName_, "Rotation_EndValue", &rotationParam_.endValue);
    globalParameter_->Regist(groupName_, "Rotation_MaxTime", &rotationParam_.maxTime);
    globalParameter_->Regist(groupName_, "Rotation_EaseType", &rotationParam_.easeType);
    globalParameter_->Regist(groupName_, "Rotation_ReturnMaxTime", &rotationParam_.returnMaxTime);
    globalParameter_->Regist(groupName_, "Rotation_ReturnEaseType", &rotationParam_.returnEaseType);

    // Translation
    globalParameter_->Regist(groupName_, "Translation_IsActive", &translationParam_.isActive);
    globalParameter_->Regist(groupName_, "Translation_UseRail", &translationParam_.useRail);
    globalParameter_->Regist(groupName_, "Translation_ReturnToOrigin", &translationParam_.returnToOrigin);
    globalParameter_->Regist(groupName_, "Translation_StartValue", &translationParam_.startValue);
    globalParameter_->Regist(groupName_, "Translation_EndValue", &translationParam_.endValue);
    globalParameter_->Regist(groupName_, "Translation_MaxTime", &translationParam_.maxTime);
    globalParameter_->Regist(groupName_, "Translation_EaseType", &translationParam_.easeType);
    globalParameter_->Regist(groupName_, "Translation_ReturnMaxTime", &translationParam_.returnMaxTime);
    globalParameter_->Regist(groupName_, "Translation_ReturnEaseType", &translationParam_.returnEaseType);
    globalParameter_->Regist(groupName_, "Translation_RailFileName", &translationParam_.railFileName);
}

void ObjEaseAnimationData::LoadParams() {
    // Scale
    scaleParam_.isActive       = globalParameter_->GetValue<bool>(groupName_, "Scale_IsActive");
    scaleParam_.returnToOrigin = globalParameter_->GetValue<bool>(groupName_, "Scale_ReturnToOrigin");
    scaleParam_.startValue     = globalParameter_->GetValue<Vector3>(groupName_, "Scale_StartValue");
    scaleParam_.endValue       = globalParameter_->GetValue<Vector3>(groupName_, "Scale_EndValue");
    scaleParam_.maxTime        = globalParameter_->GetValue<float>(groupName_, "Scale_MaxTime");
    scaleParam_.easeType       = globalParameter_->GetValue<int32_t>(groupName_, "Scale_EaseType");
    scaleParam_.returnMaxTime  = globalParameter_->GetValue<float>(groupName_, "Scale_ReturnMaxTime");
    scaleParam_.returnEaseType = globalParameter_->GetValue<int32_t>(groupName_, "Scale_ReturnEaseType");

    // Rotation
    rotationParam_.isActive       = globalParameter_->GetValue<bool>(groupName_, "Rotation_IsActive");
    rotationParam_.returnToOrigin = globalParameter_->GetValue<bool>(groupName_, "Rotation_ReturnToOrigin");
    rotationParam_.startValue     = globalParameter_->GetValue<Vector3>(groupName_, "Rotation_StartValue");
    rotationParam_.endValue       = globalParameter_->GetValue<Vector3>(groupName_, "Rotation_EndValue");
    rotationParam_.maxTime        = globalParameter_->GetValue<float>(groupName_, "Rotation_MaxTime");
    rotationParam_.easeType       = globalParameter_->GetValue<int32_t>(groupName_, "Rotation_EaseType");
    rotationParam_.returnMaxTime  = globalParameter_->GetValue<float>(groupName_, "Rotation_ReturnMaxTime");
    rotationParam_.returnEaseType = globalParameter_->GetValue<int32_t>(groupName_, "Rotation_ReturnEaseType");

    // Translation
    translationParam_.isActive       = globalParameter_->GetValue<bool>(groupName_, "Translation_IsActive");
    translationParam_.useRail        = globalParameter_->GetValue<bool>(groupName_, "Translation_UseRail");
    translationParam_.returnToOrigin = globalParameter_->GetValue<bool>(groupName_, "Translation_ReturnToOrigin");
    translationParam_.startValue     = globalParameter_->GetValue<Vector3>(groupName_, "Translation_StartValue");
    translationParam_.endValue       = globalParameter_->GetValue<Vector3>(groupName_, "Translation_EndValue");
    translationParam_.maxTime        = globalParameter_->GetValue<float>(groupName_, "Translation_MaxTime");
    translationParam_.easeType       = globalParameter_->GetValue<int32_t>(groupName_, "Translation_EaseType");
    translationParam_.returnMaxTime  = globalParameter_->GetValue<float>(groupName_, "Translation_ReturnMaxTime");
    translationParam_.returnEaseType = globalParameter_->GetValue<int32_t>(groupName_, "Translation_ReturnEaseType");
    translationParam_.railFileName   = globalParameter_->GetValue<std::string>(groupName_, "Translation_RailFileName");
}

void ObjEaseAnimationData::ResetParams() {
    playState_ = PlayState::STOPPED;

    scaleParam_.currentValue       = scaleParam_.startValue;
    rotationParam_.currentValue    = rotationParam_.startValue;
    translationParam_.currentValue = translationParam_.startValue;

    originalScale_       = scaleParam_.startValue;
    originalRotation_    = rotationParam_.startValue;
    originalTranslation_ = translationParam_.startValue;
}

void ObjEaseAnimationData::ImGuiTransformParam(const char* label, TransformParam& param, TransformType type) {
    ImGui::SeparatorText(label);
    ImGui::Checkbox((std::string(label) + " Active").c_str(), &param.isActive);

    if (!param.isActive)
        return;

    ImGui::Checkbox("Return To Origin", &param.returnToOrigin);

    // Translation専用: Rail使用フラグ
    if (type == TransformType::Translation) {
        ImGui::Checkbox("Use Rail", &param.useRail);

        if (param.useRail) {
            // Railファイル選択
            std::string directory = "Resources/GlobalParameter/RailEditor/Dates";
            railFileSelector_.selector.SelectFile("Rail File", directory, param.railFileName, "", true);
            return; // Rail使用時は通常パラメータを表示しない
        }
    }

    ImGui::DragFloat3("Start Value", &param.startValue.x, 0.01f);
    ImGui::DragFloat3("End Value", &param.endValue.x, 0.01f);
    ImGui::DragFloat("Max Time", &param.maxTime, 0.01f, 0.1f, 10.0f);
    ImGuiEasingTypeSelector("Easing Type", param.easeType);

    if (param.returnToOrigin) {
        ImGui::SeparatorText("Return Parameters");
        ImGui::DragFloat("Return Max Time", &param.returnMaxTime, 0.01f, 0.1f, 10.0f);
        ImGuiEasingTypeSelector("Return Easing Type", param.returnEaseType);
    }
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
        ImGuiTransformParam("Scale", scaleParam_, TransformType::Scale);
        ImGuiTransformParam("Rotation", rotationParam_, TransformType::Rotation);
        ImGuiTransformParam("Translation", translationParam_, TransformType::Translation);

        ImGui::PopID();
    }
#endif // _DEBUG
}