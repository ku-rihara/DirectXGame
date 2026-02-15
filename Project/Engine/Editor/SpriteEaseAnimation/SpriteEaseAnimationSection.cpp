#include "SpriteEaseAnimationSection.h"

using namespace KetaEngine;
#include "Frame/Frame.h"
#include "MathFunction.h"
#include <imgui.h>

void SpriteEaseAnimationSection::Init(const std::string& animationName, const std::string& categoryName, int32_t keyNumber) {
    globalParameter_             = GlobalParameter::GetInstance();
    currenTSequenceElementIndex_ = keyNumber;
    groupName_                   = animationName + std::to_string(currenTSequenceElementIndex_);
    folderPath_                  = "SpriteEaseAnimation/" + categoryName + "/" + "Sections/" + animationName + "/";

    // Scaleの初期値を(1,1)に設定
    scaleParam_.endValue = Vector2::OneVector();

    // Colorの初期値を(1,1,1)に設定
    colorParam_.endValue = {1.0f, 1.0f, 1.0f};

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

void SpriteEaseAnimationSection::Reset() {
    // Vec2パラメータのリセット
    auto resetVec2 = [](Vec2Param& param) {
        param.ease.Reset();
        param.returnEase.Reset();
        param.currentOffset     = param.startValue;
        param.state             = PropertyState::INACTIVE;
        param.returnElapsedTime = 0.0f;
    };

    // Vec3パラメータのリセット
    auto resetVec3 = [](Vec3Param& param) {
        param.ease.Reset();
        param.returnEase.Reset();
        param.currentOffset     = param.startValue;
        param.state             = PropertyState::INACTIVE;
        param.returnElapsedTime = 0.0f;
    };

    resetVec2(scaleParam_);
    resetVec2(positionParam_);
    resetVec3(rotationParam_);
    resetVec3(colorParam_);

    alphaParam_.ease.Reset();
    alphaParam_.returnEase.Reset();
    alphaParam_.currentOffset     = alphaParam_.startValue;
    alphaParam_.state             = PropertyState::INACTIVE;
    alphaParam_.returnElapsedTime = 0.0f;

    playState_   = PlayState::STOPPED;
    elapsedTime_ = 0.0f;

    AdaptValueSetting();
    AdaptEaseParam();
}

void SpriteEaseAnimationSection::LoadData() {
    globalParameter_->LoadFile(groupName_, folderPath_);
    globalParameter_->SyncParamForGroup(groupName_);
}

void SpriteEaseAnimationSection::SaveData() {
    globalParameter_->SaveFile(groupName_, folderPath_);
}

void SpriteEaseAnimationSection::Update(float speedRate) {
    float actualDeltaTime;

    switch (static_cast<TimeMode>(timeModeSelector_.GetTimeModeInt())) {
    case TimeMode::DELTA_TIME:
        actualDeltaTime = Frame::DeltaTime() * speedRate;
        break;
    case TimeMode::DELTA_TIME_RATE:
    default:
        actualDeltaTime = Frame::DeltaTimeRate() * speedRate;
        break;
    }

    if (playState_ == PlayState::STOPPED) {
        return;
    }

    if (playState_ == PlayState::WAITING) {
        elapsedTime_ += actualDeltaTime;
        if (elapsedTime_ >= startTime_) {
            StartPlay();
        }
        return;
    }

    // 各プロパティを個別に更新
    if (scaleParam_.isActive) {
        UpdateVec2Property(scaleParam_, actualDeltaTime);
    }
    if (positionParam_.isActive) {
        UpdateVec2Property(positionParam_, actualDeltaTime);
    }
    if (rotationParam_.isActive) {
        UpdateVec3Property(rotationParam_, actualDeltaTime);
    }
    if (colorParam_.isActive) {
        UpdateVec3Property(colorParam_, actualDeltaTime);
    }
    if (alphaParam_.isActive) {
        UpdateFloatProperty(alphaParam_, actualDeltaTime);
    }

    UpdateOverallState();
}

// ======================== Vec2 Update ========================

void SpriteEaseAnimationSection::UpdateVec2Property(Vec2Param& param, float deltaTime) {
    switch (param.state) {
    case PropertyState::INACTIVE:
    case PropertyState::WAITING:
    case PropertyState::FINISHED:
        break;

    case PropertyState::PLAYING:
        UpdateVec2Play(param, deltaTime);
        break;

    case PropertyState::RETURN_WAITING:
        param.returnElapsedTime += deltaTime;
        if (param.returnElapsedTime >= returnStartTime_) {
            param.returnEase.SetAdaptValue(&param.currentOffset);
            param.returnEase.SetStartValue(param.currentOffset);
            param.returnEase.SetEndValue(param.startValue);
            param.returnEase.SetMaxTime(param.returnMaxTime);
            param.returnEase.SetType(static_cast<EasingType>(param.returnEaseType));
            param.returnEase.SetIsStartEndReverse(false);
            param.returnEase.Reset();
            param.state = PropertyState::RETURNING;
        }
        break;

    case PropertyState::RETURNING:
        UpdateVec2Return(param, deltaTime);
        break;
    }
}

void SpriteEaseAnimationSection::UpdateVec2Play(Vec2Param& param, float deltaTime) {
    param.ease.Update(deltaTime);
    param.currentOffset = param.ease.GetValue();

    if (param.ease.IsFinished()) {
        if (param.isReturnToOrigin) {
            param.state             = PropertyState::RETURN_WAITING;
            param.returnElapsedTime = 0.0f;
        } else {
            param.state = PropertyState::FINISHED;
        }
    }
}

void SpriteEaseAnimationSection::UpdateVec2Return(Vec2Param& param, float deltaTime) {
    param.returnEase.Update(deltaTime);
    param.currentOffset = param.returnEase.GetValue();

    if (param.returnEase.IsFinished()) {
        param.state = PropertyState::FINISHED;
    }
}

// ======================== Vec3 Update ========================

void SpriteEaseAnimationSection::UpdateVec3Property(Vec3Param& param, float deltaTime) {
    switch (param.state) {
    case PropertyState::INACTIVE:
    case PropertyState::WAITING:
    case PropertyState::FINISHED:
        break;

    case PropertyState::PLAYING:
        UpdateVec3Play(param, deltaTime);
        break;

    case PropertyState::RETURN_WAITING:
        param.returnElapsedTime += deltaTime;
        if (param.returnElapsedTime >= returnStartTime_) {
            param.returnEase.SetAdaptValue(&param.currentOffset);
            param.returnEase.SetStartValue(param.currentOffset);
            param.returnEase.SetEndValue(param.startValue);
            param.returnEase.SetMaxTime(param.returnMaxTime);
            param.returnEase.SetType(static_cast<EasingType>(param.returnEaseType));
            param.returnEase.SetIsStartEndReverse(false);
            param.returnEase.Reset();
            param.state = PropertyState::RETURNING;
        }
        break;

    case PropertyState::RETURNING:
        UpdateVec3Return(param, deltaTime);
        break;
    }
}

void SpriteEaseAnimationSection::UpdateVec3Play(Vec3Param& param, float deltaTime) {
    param.ease.Update(deltaTime);
    param.currentOffset = param.ease.GetValue();

    if (param.ease.IsFinished()) {
        if (param.isReturnToOrigin) {
            param.state             = PropertyState::RETURN_WAITING;
            param.returnElapsedTime = 0.0f;
        } else {
            param.state = PropertyState::FINISHED;
        }
    }
}

void SpriteEaseAnimationSection::UpdateVec3Return(Vec3Param& param, float deltaTime) {
    param.returnEase.Update(deltaTime);
    param.currentOffset = param.returnEase.GetValue();

    if (param.returnEase.IsFinished()) {
        param.state = PropertyState::FINISHED;
    }
}

// ======================== Float Update ========================

void SpriteEaseAnimationSection::UpdateFloatProperty(FloatParam& param, float deltaTime) {
    switch (param.state) {
    case PropertyState::INACTIVE:
    case PropertyState::WAITING:
    case PropertyState::FINISHED:
        break;

    case PropertyState::PLAYING:
        UpdateFloatPlay(param, deltaTime);
        break;

    case PropertyState::RETURN_WAITING:
        param.returnElapsedTime += deltaTime;
        if (param.returnElapsedTime >= returnStartTime_) {
            param.returnEase.SetAdaptValue(&param.currentOffset);
            param.returnEase.SetStartValue(param.currentOffset);
            param.returnEase.SetEndValue(param.startValue);
            param.returnEase.SetMaxTime(param.returnMaxTime);
            param.returnEase.SetType(static_cast<EasingType>(param.returnEaseType));
            param.returnEase.SetIsStartEndReverse(false);
            param.returnEase.Reset();
            param.state = PropertyState::RETURNING;
        }
        break;

    case PropertyState::RETURNING:
        UpdateFloatReturn(param, deltaTime);
        break;
    }
}

void SpriteEaseAnimationSection::UpdateFloatPlay(FloatParam& param, float deltaTime) {
    param.ease.Update(deltaTime);
    param.currentOffset = param.ease.GetValue();

    if (param.ease.IsFinished()) {
        if (param.isReturnToOrigin) {
            param.state             = PropertyState::RETURN_WAITING;
            param.returnElapsedTime = 0.0f;
        } else {
            param.state = PropertyState::FINISHED;
        }
    }
}

void SpriteEaseAnimationSection::UpdateFloatReturn(FloatParam& param, float deltaTime) {
    param.returnEase.Update(deltaTime);
    param.currentOffset = param.returnEase.GetValue();

    if (param.returnEase.IsFinished()) {
        param.state = PropertyState::FINISHED;
    }
}

// ======================== Overall State ========================

void SpriteEaseAnimationSection::UpdateOverallState() {
    if (AreAllPropertiesFinished()) {
        playState_ = PlayState::STOPPED;
    } else {
        bool anyPlaying   = false;
        bool anyReturning = false;

        auto checkVec2 = [&](const Vec2Param& p) {
            if (!p.isActive) return;
            if (p.state == PropertyState::PLAYING) anyPlaying = true;
            if (p.state == PropertyState::RETURNING || p.state == PropertyState::RETURN_WAITING) anyReturning = true;
        };
        auto checkVec3 = [&](const Vec3Param& p) {
            if (!p.isActive) return;
            if (p.state == PropertyState::PLAYING) anyPlaying = true;
            if (p.state == PropertyState::RETURNING || p.state == PropertyState::RETURN_WAITING) anyReturning = true;
        };

        checkVec2(scaleParam_);
        checkVec2(positionParam_);
        checkVec3(rotationParam_);
        checkVec3(colorParam_);

        if (alphaParam_.isActive) {
            if (alphaParam_.state == PropertyState::PLAYING) anyPlaying = true;
            if (alphaParam_.state == PropertyState::RETURNING || alphaParam_.state == PropertyState::RETURN_WAITING) anyReturning = true;
        }

        if (anyReturning) {
            playState_ = PlayState::RETURNING;
        } else if (anyPlaying) {
            playState_ = PlayState::PLAYING;
        }
    }
}

bool SpriteEaseAnimationSection::AreAllPropertiesFinished() const {
    auto isVec2Finished = [](const Vec2Param& p) {
        return !p.isActive || p.state == PropertyState::FINISHED;
    };
    auto isVec3Finished = [](const Vec3Param& p) {
        return !p.isActive || p.state == PropertyState::FINISHED;
    };

    if (!isVec2Finished(scaleParam_)) return false;
    if (!isVec2Finished(positionParam_)) return false;
    if (!isVec3Finished(rotationParam_)) return false;
    if (!isVec3Finished(colorParam_)) return false;
    if (alphaParam_.isActive && alphaParam_.state != PropertyState::FINISHED) return false;

    return true;
}

void SpriteEaseAnimationSection::StartPlay() {
    playState_ = PlayState::PLAYING;

    auto startVec2 = [](Vec2Param& p) {
        if (p.isActive) {
            p.currentOffset     = p.startValue;
            p.ease.Reset();
            p.state             = PropertyState::PLAYING;
            p.returnElapsedTime = 0.0f;
        }
    };
    auto startVec3 = [](Vec3Param& p) {
        if (p.isActive) {
            p.currentOffset     = p.startValue;
            p.ease.Reset();
            p.state             = PropertyState::PLAYING;
            p.returnElapsedTime = 0.0f;
        }
    };

    startVec2(scaleParam_);
    startVec2(positionParam_);
    startVec3(rotationParam_);
    startVec3(colorParam_);

    if (alphaParam_.isActive) {
        alphaParam_.currentOffset     = alphaParam_.startValue;
        alphaParam_.ease.Reset();
        alphaParam_.state             = PropertyState::PLAYING;
        alphaParam_.returnElapsedTime = 0.0f;
    }
}

// ======================== Params ========================

void SpriteEaseAnimationSection::RegisterParams() {
    globalParameter_->Regist(groupName_, "startTime", &startTime_);
    globalParameter_->Regist(groupName_, "returnStartTime", &returnStartTime_);
    timeModeSelector_.RegisterParam(groupName_, globalParameter_);

    // Scale (Vec2)
    globalParameter_->Regist(groupName_, "Scale_IsActive", &scaleParam_.isActive);
    globalParameter_->Regist(groupName_, "Scale_IsReturnToOrigin", &scaleParam_.isReturnToOrigin);
    globalParameter_->Regist(groupName_, "Scale_endValue", &scaleParam_.endValue);
    globalParameter_->Regist(groupName_, "Scale_MaxTime", &scaleParam_.maxTime);
    globalParameter_->Regist(groupName_, "Scale_EaseType", &scaleParam_.easeType);
    globalParameter_->Regist(groupName_, "Scale_ReturnMaxTime", &scaleParam_.returnMaxTime);
    globalParameter_->Regist(groupName_, "Scale_ReturnEaseType", &scaleParam_.returnEaseType);

    // Position (Vec2)
    globalParameter_->Regist(groupName_, "Position_IsActive", &positionParam_.isActive);
    globalParameter_->Regist(groupName_, "Position_IsReturnToOrigin", &positionParam_.isReturnToOrigin);
    globalParameter_->Regist(groupName_, "Position_endValue", &positionParam_.endValue);
    globalParameter_->Regist(groupName_, "Position_MaxTime", &positionParam_.maxTime);
    globalParameter_->Regist(groupName_, "Position_EaseType", &positionParam_.easeType);
    globalParameter_->Regist(groupName_, "Position_ReturnMaxTime", &positionParam_.returnMaxTime);
    globalParameter_->Regist(groupName_, "Position_ReturnEaseType", &positionParam_.returnEaseType);

    // Rotation (Vec3)
    globalParameter_->Regist(groupName_, "Rotation_IsActive", &rotationParam_.isActive);
    globalParameter_->Regist(groupName_, "Rotation_IsReturnToOrigin", &rotationParam_.isReturnToOrigin);
    globalParameter_->Regist(groupName_, "Rotation_endValue", &rotationParam_.endValue);
    globalParameter_->Regist(groupName_, "Rotation_MaxTime", &rotationParam_.maxTime);
    globalParameter_->Regist(groupName_, "Rotation_EaseType", &rotationParam_.easeType);
    globalParameter_->Regist(groupName_, "Rotation_ReturnMaxTime", &rotationParam_.returnMaxTime);
    globalParameter_->Regist(groupName_, "Rotation_ReturnEaseType", &rotationParam_.returnEaseType);

    // Color (Vec3)
    globalParameter_->Regist(groupName_, "Color_IsActive", &colorParam_.isActive);
    globalParameter_->Regist(groupName_, "Color_IsReturnToOrigin", &colorParam_.isReturnToOrigin);
    globalParameter_->Regist(groupName_, "Color_endValue", &colorParam_.endValue);
    globalParameter_->Regist(groupName_, "Color_MaxTime", &colorParam_.maxTime);
    globalParameter_->Regist(groupName_, "Color_EaseType", &colorParam_.easeType);
    globalParameter_->Regist(groupName_, "Color_ReturnMaxTime", &colorParam_.returnMaxTime);
    globalParameter_->Regist(groupName_, "Color_ReturnEaseType", &colorParam_.returnEaseType);

    // Alpha (float)
    globalParameter_->Regist(groupName_, "Alpha_IsActive", &alphaParam_.isActive);
    globalParameter_->Regist(groupName_, "Alpha_IsReturnToOrigin", &alphaParam_.isReturnToOrigin);
    globalParameter_->Regist(groupName_, "Alpha_endValue", &alphaParam_.endValue);
    globalParameter_->Regist(groupName_, "Alpha_MaxTime", &alphaParam_.maxTime);
    globalParameter_->Regist(groupName_, "Alpha_EaseType", &alphaParam_.easeType);
    globalParameter_->Regist(groupName_, "Alpha_ReturnMaxTime", &alphaParam_.returnMaxTime);
    globalParameter_->Regist(groupName_, "Alpha_ReturnEaseType", &alphaParam_.returnEaseType);
}

void SpriteEaseAnimationSection::GetParams() {
    startTime_       = globalParameter_->GetValue<float>(groupName_, "startTime");
    returnStartTime_ = globalParameter_->GetValue<float>(groupName_, "returnStartTime");
    timeModeSelector_.GetParam(groupName_, globalParameter_);

    // Scale
    scaleParam_.isActive         = globalParameter_->GetValue<bool>(groupName_, "Scale_IsActive");
    scaleParam_.isReturnToOrigin = globalParameter_->GetValue<bool>(groupName_, "Scale_IsReturnToOrigin");
    scaleParam_.endValue         = globalParameter_->GetValue<Vector2>(groupName_, "Scale_endValue");
    scaleParam_.maxTime          = globalParameter_->GetValue<float>(groupName_, "Scale_MaxTime");
    scaleParam_.easeType         = globalParameter_->GetValue<int32_t>(groupName_, "Scale_EaseType");
    scaleParam_.returnMaxTime    = globalParameter_->GetValue<float>(groupName_, "Scale_ReturnMaxTime");
    scaleParam_.returnEaseType   = globalParameter_->GetValue<int32_t>(groupName_, "Scale_ReturnEaseType");

    // Position
    positionParam_.isActive         = globalParameter_->GetValue<bool>(groupName_, "Position_IsActive");
    positionParam_.isReturnToOrigin = globalParameter_->GetValue<bool>(groupName_, "Position_IsReturnToOrigin");
    positionParam_.endValue         = globalParameter_->GetValue<Vector2>(groupName_, "Position_endValue");
    positionParam_.maxTime          = globalParameter_->GetValue<float>(groupName_, "Position_MaxTime");
    positionParam_.easeType         = globalParameter_->GetValue<int32_t>(groupName_, "Position_EaseType");
    positionParam_.returnMaxTime    = globalParameter_->GetValue<float>(groupName_, "Position_ReturnMaxTime");
    positionParam_.returnEaseType   = globalParameter_->GetValue<int32_t>(groupName_, "Position_ReturnEaseType");

    // Rotation
    rotationParam_.isActive         = globalParameter_->GetValue<bool>(groupName_, "Rotation_IsActive");
    rotationParam_.isReturnToOrigin = globalParameter_->GetValue<bool>(groupName_, "Rotation_IsReturnToOrigin");
    rotationParam_.endValue         = globalParameter_->GetValue<Vector3>(groupName_, "Rotation_endValue");
    rotationParam_.maxTime          = globalParameter_->GetValue<float>(groupName_, "Rotation_MaxTime");
    rotationParam_.easeType         = globalParameter_->GetValue<int32_t>(groupName_, "Rotation_EaseType");
    rotationParam_.returnMaxTime    = globalParameter_->GetValue<float>(groupName_, "Rotation_ReturnMaxTime");
    rotationParam_.returnEaseType   = globalParameter_->GetValue<int32_t>(groupName_, "Rotation_ReturnEaseType");

    // Color
    colorParam_.isActive         = globalParameter_->GetValue<bool>(groupName_, "Color_IsActive");
    colorParam_.isReturnToOrigin = globalParameter_->GetValue<bool>(groupName_, "Color_IsReturnToOrigin");
    colorParam_.endValue         = globalParameter_->GetValue<Vector3>(groupName_, "Color_endValue");
    colorParam_.maxTime          = globalParameter_->GetValue<float>(groupName_, "Color_MaxTime");
    colorParam_.easeType         = globalParameter_->GetValue<int32_t>(groupName_, "Color_EaseType");
    colorParam_.returnMaxTime    = globalParameter_->GetValue<float>(groupName_, "Color_ReturnMaxTime");
    colorParam_.returnEaseType   = globalParameter_->GetValue<int32_t>(groupName_, "Color_ReturnEaseType");

    // Alpha
    alphaParam_.isActive         = globalParameter_->GetValue<bool>(groupName_, "Alpha_IsActive");
    alphaParam_.isReturnToOrigin = globalParameter_->GetValue<bool>(groupName_, "Alpha_IsReturnToOrigin");
    alphaParam_.endValue         = globalParameter_->GetValue<float>(groupName_, "Alpha_endValue");
    alphaParam_.maxTime          = globalParameter_->GetValue<float>(groupName_, "Alpha_MaxTime");
    alphaParam_.easeType         = globalParameter_->GetValue<int32_t>(groupName_, "Alpha_EaseType");
    alphaParam_.returnMaxTime    = globalParameter_->GetValue<float>(groupName_, "Alpha_ReturnMaxTime");
    alphaParam_.returnEaseType   = globalParameter_->GetValue<int32_t>(groupName_, "Alpha_ReturnEaseType");
}

void SpriteEaseAnimationSection::AdaptEaseParam() {
    // Scale
    scaleParam_.ease.SetMaxTime(scaleParam_.maxTime);
    scaleParam_.ease.SetEndValue(scaleParam_.endValue);
    scaleParam_.ease.SetType(static_cast<EasingType>(scaleParam_.easeType));
    scaleParam_.ease.SetIsStartEndReverse(false);

    // Position
    positionParam_.ease.SetMaxTime(positionParam_.maxTime);
    positionParam_.ease.SetEndValue(positionParam_.endValue);
    positionParam_.ease.SetType(static_cast<EasingType>(positionParam_.easeType));
    positionParam_.ease.SetIsStartEndReverse(false);

    // Rotation
    rotationParam_.ease.SetMaxTime(rotationParam_.maxTime);
    rotationParam_.ease.SetEndValue(rotationParam_.endValue);
    rotationParam_.ease.SetType(static_cast<EasingType>(rotationParam_.easeType));
    rotationParam_.ease.SetIsStartEndReverse(false);

    // Color
    colorParam_.ease.SetMaxTime(colorParam_.maxTime);
    colorParam_.ease.SetEndValue(colorParam_.endValue);
    colorParam_.ease.SetType(static_cast<EasingType>(colorParam_.easeType));
    colorParam_.ease.SetIsStartEndReverse(false);

    // Alpha
    alphaParam_.ease.SetMaxTime(alphaParam_.maxTime);
    alphaParam_.ease.SetEndValue(alphaParam_.endValue);
    alphaParam_.ease.SetType(static_cast<EasingType>(alphaParam_.easeType));
    alphaParam_.ease.SetIsStartEndReverse(false);
}

void SpriteEaseAnimationSection::AdaptValueSetting() {
    scaleParam_.ease.SetAdaptValue(&scaleParam_.currentOffset);
    positionParam_.ease.SetAdaptValue(&positionParam_.currentOffset);
    rotationParam_.ease.SetAdaptValue(&rotationParam_.currentOffset);
    colorParam_.ease.SetAdaptValue(&colorParam_.currentOffset);
    alphaParam_.ease.SetAdaptValue(&alphaParam_.currentOffset);
}

// ======================== ImGui ========================

void SpriteEaseAnimationSection::ImGuiVec2Param(const char* label, Vec2Param& param) {
    ImGui::SeparatorText(label);
    ImGui::Checkbox((std::string(label) + " Active").c_str(), &param.isActive);
    ImGui::PushID(label);

    if (!param.isActive) {
        ImGui::PopID();
        return;
    }

    ImGui::Checkbox((std::string(label) + " Return To Origin").c_str(), &param.isReturnToOrigin);
    ImGui::DragFloat2("End Value", &param.endValue.x, 0.01f);
    ImGui::DragFloat("Max Time", &param.maxTime, 0.01f, 0.0f, 10.0f);
    ImGuiEasingTypeSelector("Easing Type", param.easeType);

    if (param.isReturnToOrigin) {
        ImGui::Separator();
        ImGui::Text("Return Settings:");
        ImGui::DragFloat("Return Start Time", &returnStartTime_, 0.01f, 0.0f, 100.0f);
        ImGui::DragFloat("Return Max Time", &param.returnMaxTime, 0.01f, 0.0f, 10.0f);
        ImGuiEasingTypeSelector("Return Easing Type", param.returnEaseType);
    }

    ImGui::PopID();
}

void SpriteEaseAnimationSection::ImGuiVec3Param(const char* label, Vec3Param& param, bool isRotation) {
    ImGui::SeparatorText(label);
    ImGui::Checkbox((std::string(label) + " Active").c_str(), &param.isActive);
    ImGui::PushID(label);

    if (!param.isActive) {
        ImGui::PopID();
        return;
    }

    ImGui::Checkbox((std::string(label) + " Return To Origin").c_str(), &param.isReturnToOrigin);

    if (isRotation) {
        Vector3 endValueDeg = ToDegree(param.endValue);
        ImGui::DragFloat3("End Value (Degree)", &endValueDeg.x, 0.1f);
        param.endValue = ToRadian(endValueDeg);
    } else {
        ImGui::DragFloat3("End Value", &param.endValue.x, 0.01f);
    }

    ImGui::DragFloat("Max Time", &param.maxTime, 0.01f, 0.0f, 10.0f);
    ImGuiEasingTypeSelector("Easing Type", param.easeType);

    if (param.isReturnToOrigin) {
        ImGui::Separator();
        ImGui::Text("Return Settings:");
        ImGui::DragFloat("Return Start Time", &returnStartTime_, 0.01f, 0.0f, 100.0f);
        ImGui::DragFloat("Return Max Time", &param.returnMaxTime, 0.01f, 0.0f, 10.0f);
        ImGuiEasingTypeSelector("Return Easing Type", param.returnEaseType);
    }

    ImGui::PopID();
}

void SpriteEaseAnimationSection::ImGuiColorParam(const char* label, Vec3Param& param) {
    ImGui::SeparatorText(label);
    ImGui::Checkbox((std::string(label) + " Active").c_str(), &param.isActive);
    ImGui::PushID(label);

    if (!param.isActive) {
        ImGui::PopID();
        return;
    }

    ImGui::Checkbox((std::string(label) + " Return To Origin").c_str(), &param.isReturnToOrigin);
    ImGui::ColorEdit3("End Value", &param.endValue.x);
    ImGui::DragFloat("Max Time", &param.maxTime, 0.01f, 0.0f, 10.0f);
    ImGuiEasingTypeSelector("Easing Type", param.easeType);

    if (param.isReturnToOrigin) {
        ImGui::Separator();
        ImGui::Text("Return Settings:");
        ImGui::DragFloat("Return Start Time", &returnStartTime_, 0.01f, 0.0f, 100.0f);
        ImGui::DragFloat("Return Max Time", &param.returnMaxTime, 0.01f, 0.0f, 10.0f);
        ImGuiEasingTypeSelector("Return Easing Type", param.returnEaseType);
    }

    ImGui::PopID();
}

void SpriteEaseAnimationSection::ImGuiFloatParam(const char* label, FloatParam& param, bool isAlpha) {
    ImGui::SeparatorText(label);
    ImGui::Checkbox((std::string(label) + " Active").c_str(), &param.isActive);
    ImGui::PushID(label);

    if (!param.isActive) {
        ImGui::PopID();
        return;
    }

    ImGui::Checkbox((std::string(label) + " Return To Origin").c_str(), &param.isReturnToOrigin);

    if (isAlpha) {
        ImGui::SliderFloat("End Value", &param.endValue, 0.0f, 1.0f);
    } else {
        ImGui::DragFloat("End Value", &param.endValue, 0.01f);
    }

    ImGui::DragFloat("Max Time", &param.maxTime, 0.01f, 0.0f, 10.0f);
    ImGuiEasingTypeSelector("Easing Type", param.easeType);

    if (param.isReturnToOrigin) {
        ImGui::Separator();
        ImGui::Text("Return Settings:");
        ImGui::DragFloat("Return Start Time", &returnStartTime_, 0.01f, 0.0f, 100.0f);
        ImGui::DragFloat("Return Max Time", &param.returnMaxTime, 0.01f, 0.0f, 10.0f);
        ImGuiEasingTypeSelector("Return Easing Type", param.returnEaseType);
    }

    ImGui::PopID();
}

void SpriteEaseAnimationSection::AdjustParam() {
#ifdef _DEBUG
    ImGui::SeparatorText(("Section: " + groupName_).c_str());
    ImGui::PushID(groupName_.c_str());

    ImGui::DragFloat("Start Time", &startTime_, 0.01f, 0.0f, 100.0f);
    timeModeSelector_.SelectTimeModeImGui("Time Mode");

    ImGui::Separator();

    ImGuiVec2Param("Scale", scaleParam_);
    ImGuiVec2Param("Position", positionParam_);
    ImGuiVec3Param("Rotation", rotationParam_, true);
    ImGuiColorParam("Color", colorParam_);
    ImGuiFloatParam("Alpha", alphaParam_, true);

    AdaptEaseParam();

    ImGui::PopID();
#endif
}

// ======================== Start Values ========================

void SpriteEaseAnimationSection::SetStartValues(const Vector2& scale, const Vector2& position,
                                                 const Vector3& rotation, const Vector3& color, float alpha) {
    scaleParam_.ease.SetStartValue(scale);
    scaleParam_.startValue    = scale;
    scaleParam_.currentOffset = scale;

    positionParam_.ease.SetStartValue(position);
    positionParam_.startValue    = position;
    positionParam_.currentOffset = position;

    rotationParam_.ease.SetStartValue(rotation);
    rotationParam_.startValue    = rotation;
    rotationParam_.currentOffset = rotation;

    colorParam_.ease.SetStartValue(color);
    colorParam_.startValue    = color;
    colorParam_.currentOffset = color;

    alphaParam_.ease.SetStartValue(alpha);
    alphaParam_.startValue    = alpha;
    alphaParam_.currentOffset = alpha;
}

void SpriteEaseAnimationSection::SetPreAnimationOffsets(const Vector2& scale, const Vector2& position,
                                                        const Vector3& rotation, const Vector3& color, float alpha) {
    scaleParam_.preAnimationOffset    = scale;
    positionParam_.preAnimationOffset = position;
    rotationParam_.preAnimationOffset = rotation;
    colorParam_.preAnimationOffset    = color;
    alphaParam_.preAnimationOffset    = alpha;
}

void SpriteEaseAnimationSection::StartWaiting() {
    playState_   = PlayState::WAITING;
    elapsedTime_ = 0.0f;

    auto waitVec2 = [](Vec2Param& p) {
        if (p.isActive) {
            p.state             = PropertyState::WAITING;
            p.returnElapsedTime = 0.0f;
            p.currentOffset     = p.preAnimationOffset;
        }
    };
    auto waitVec3 = [](Vec3Param& p) {
        if (p.isActive) {
            p.state             = PropertyState::WAITING;
            p.returnElapsedTime = 0.0f;
            p.currentOffset     = p.preAnimationOffset;
        }
    };

    waitVec2(scaleParam_);
    waitVec2(positionParam_);
    waitVec3(rotationParam_);
    waitVec3(colorParam_);

    if (alphaParam_.isActive) {
        alphaParam_.state             = PropertyState::WAITING;
        alphaParam_.returnElapsedTime = 0.0f;
        alphaParam_.currentOffset     = alphaParam_.preAnimationOffset;
    }
}

bool SpriteEaseAnimationSection::IsFinished() const {
    return AreAllPropertiesFinished();
}

bool SpriteEaseAnimationSection::IsPropertyFinished(PropertyType type) const {
    switch (type) {
    case PropertyType::Scale:
        return !scaleParam_.isActive || scaleParam_.state == PropertyState::FINISHED;
    case PropertyType::Position:
        return !positionParam_.isActive || positionParam_.state == PropertyState::FINISHED;
    case PropertyType::Rotation:
        return !rotationParam_.isActive || rotationParam_.state == PropertyState::FINISHED;
    case PropertyType::Color:
        return !colorParam_.isActive || colorParam_.state == PropertyState::FINISHED;
    case PropertyType::Alpha:
        return !alphaParam_.isActive || alphaParam_.state == PropertyState::FINISHED;
    default:
        return true;
    }
}

bool SpriteEaseAnimationSection::IsPropertyActive(PropertyType type) const {
    switch (type) {
    case PropertyType::Scale:    return scaleParam_.isActive;
    case PropertyType::Position: return positionParam_.isActive;
    case PropertyType::Rotation: return rotationParam_.isActive;
    case PropertyType::Color:    return colorParam_.isActive;
    case PropertyType::Alpha:    return alphaParam_.isActive;
    default:                     return false;
    }
}

void SpriteEaseAnimationSection::SetStartValueForProperty_Vec2(PropertyType type, const Vector2& value) {
    Vec2Param* param = nullptr;
    switch (type) {
    case PropertyType::Scale:    param = &scaleParam_;    break;
    case PropertyType::Position: param = &positionParam_; break;
    default: return;
    }

    param->ease.SetStartValue(value);
    param->startValue    = value;
    param->currentOffset = value;
}

void SpriteEaseAnimationSection::SetStartValueForProperty_Vec3(PropertyType type, const Vector3& value) {
    Vec3Param* param = nullptr;
    switch (type) {
    case PropertyType::Rotation: param = &rotationParam_; break;
    case PropertyType::Color:    param = &colorParam_;    break;
    default: return;
    }

    param->ease.SetStartValue(value);
    param->startValue    = value;
    param->currentOffset = value;
}

void SpriteEaseAnimationSection::SetStartValueForProperty_Float(PropertyType type, float value) {
    if (type != PropertyType::Alpha) return;

    alphaParam_.ease.SetStartValue(value);
    alphaParam_.startValue    = value;
    alphaParam_.currentOffset = value;
}

void SpriteEaseAnimationSection::StartWaitingForProperty(PropertyType type) {
    switch (type) {
    case PropertyType::Scale:
        if (scaleParam_.isActive) {
            scaleParam_.state             = PropertyState::WAITING;
            scaleParam_.returnElapsedTime = 0.0f;
            scaleParam_.currentOffset     = scaleParam_.preAnimationOffset;
        }
        break;
    case PropertyType::Position:
        if (positionParam_.isActive) {
            positionParam_.state             = PropertyState::WAITING;
            positionParam_.returnElapsedTime = 0.0f;
            positionParam_.currentOffset     = positionParam_.preAnimationOffset;
        }
        break;
    case PropertyType::Rotation:
        if (rotationParam_.isActive) {
            rotationParam_.state             = PropertyState::WAITING;
            rotationParam_.returnElapsedTime = 0.0f;
            rotationParam_.currentOffset     = rotationParam_.preAnimationOffset;
        }
        break;
    case PropertyType::Color:
        if (colorParam_.isActive) {
            colorParam_.state             = PropertyState::WAITING;
            colorParam_.returnElapsedTime = 0.0f;
            colorParam_.currentOffset     = colorParam_.preAnimationOffset;
        }
        break;
    case PropertyType::Alpha:
        if (alphaParam_.isActive) {
            alphaParam_.state             = PropertyState::WAITING;
            alphaParam_.returnElapsedTime = 0.0f;
            alphaParam_.currentOffset     = alphaParam_.preAnimationOffset;
        }
        break;
    default:
        break;
    }
}

void SpriteEaseAnimationSection::StartPlayingForProperty(PropertyType type) {
    playState_ = PlayState::PLAYING;

    switch (type) {
    case PropertyType::Scale:
        if (scaleParam_.isActive) {
            scaleParam_.currentOffset     = scaleParam_.startValue;
            scaleParam_.ease.Reset();
            scaleParam_.state             = PropertyState::PLAYING;
            scaleParam_.returnElapsedTime = 0.0f;
        }
        break;
    case PropertyType::Position:
        if (positionParam_.isActive) {
            positionParam_.currentOffset     = positionParam_.startValue;
            positionParam_.ease.Reset();
            positionParam_.state             = PropertyState::PLAYING;
            positionParam_.returnElapsedTime = 0.0f;
        }
        break;
    case PropertyType::Rotation:
        if (rotationParam_.isActive) {
            rotationParam_.currentOffset     = rotationParam_.startValue;
            rotationParam_.ease.Reset();
            rotationParam_.state             = PropertyState::PLAYING;
            rotationParam_.returnElapsedTime = 0.0f;
        }
        break;
    case PropertyType::Color:
        if (colorParam_.isActive) {
            colorParam_.currentOffset     = colorParam_.startValue;
            colorParam_.ease.Reset();
            colorParam_.state             = PropertyState::PLAYING;
            colorParam_.returnElapsedTime = 0.0f;
        }
        break;
    case PropertyType::Alpha:
        if (alphaParam_.isActive) {
            alphaParam_.currentOffset     = alphaParam_.startValue;
            alphaParam_.ease.Reset();
            alphaParam_.state             = PropertyState::PLAYING;
            alphaParam_.returnElapsedTime = 0.0f;
        }
        break;
    default:
        break;
    }
}

const char* SpriteEaseAnimationSection::GetPropertyName(PropertyType type) const {
    switch (type) {
    case PropertyType::Scale:    return "Scale";
    case PropertyType::Position: return "Position";
    case PropertyType::Rotation: return "Rotation";
    case PropertyType::Color:    return "Color";
    case PropertyType::Alpha:    return "Alpha";
    default:                     return "Unknown";
    }
}
