#include "TimeScalePlayer.h"

using namespace KetaEngine;
#include "Frame/Frame.h"

void TimeScalePlayer::Init() {
    BaseEffectPlayer::Init();
}

void TimeScalePlayer::Update(float deltaTime) {
    if (effectData_) {
        effectData_->Update(deltaTime);
    }
}

void TimeScalePlayer::Play(const std::string& timeScaleName) {
    currentEffectName_ = timeScaleName;

    effectData_.reset();
    effectData_ = CreateEffectData();

    effectData_->Init(timeScaleName);
    effectData_->LoadData();
    effectData_->Play();
}

void TimeScalePlayer::SetTimeScaleImmediate(float timeScale) {
    Frame::SetTimeScale(timeScale);
}

float TimeScalePlayer::GetCurrentTimeScale() const {
    return Frame::GetTimeScale();
}

float TimeScalePlayer::GetElapsedTime() const {
    if (TimeScaleData* timeScaleData = dynamic_cast<TimeScaleData*>(effectData_.get())) {
        return timeScaleData->GetElapsedTime();
    }
    return 0.0f;
}

float TimeScalePlayer::GetRemainingTime() const {
    if (TimeScaleData* timeScaleData = dynamic_cast<TimeScaleData*>(effectData_.get())) {
        return timeScaleData->GetRemainingTime();
    }
    return 0.0f;
}

std::unique_ptr<BaseEffectData> TimeScalePlayer::CreateEffectData() {
    return std::make_unique<TimeScaleData>();
}