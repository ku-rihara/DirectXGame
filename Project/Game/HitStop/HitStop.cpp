#include "HitStop.h"
#include"Frame/Frame.h" 

void HitStop::Init() {
	timeScaleController_ = std::make_unique<TimeScaleController>();
    timeScaleController_->Init();
}

void HitStop::Update() {
    timeScaleController_->Update(Frame::DeltaTime());
}

void HitStop::PlayHitStop(const std::string& timeScaleName) {
    timeScaleController_->PlayTimeScale(timeScaleName);
}


void HitStop::EditorUpdate() {
    timeScaleController_->EditorUpdate();
 }