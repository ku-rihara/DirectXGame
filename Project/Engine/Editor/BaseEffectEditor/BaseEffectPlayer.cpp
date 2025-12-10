#include "BaseEffectPlayer.h"

using namespace KetaEngine;
#include "BaseEffectData.h"

void BaseEffectPlayer::Init() {
    effectData_ = CreateEffectData();
}

void BaseEffectPlayer::Stop() {
    if (effectData_) {
        effectData_->Pause();
    }
}

void BaseEffectPlayer::Reset() {
    if (effectData_) {
        effectData_->Reset();
    }
}

bool BaseEffectPlayer::IsPlaying() const {
    return effectData_ && effectData_->IsPlaying();
}

bool BaseEffectPlayer::IsFinished() const {
    return effectData_ && effectData_->IsFinished();
}

bool BaseEffectPlayer::IsReturning() const {
    return effectData_ && effectData_->IsReturning();
}
