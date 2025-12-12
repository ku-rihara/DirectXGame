#include "AttackEffect.h"
#include"Frame/Frame.h" 

void AttackEffect::Init() {
    timeScaleController_  = std::make_unique<KetaEngine::TimeScaleController>();
    postEffectController_ = std::make_unique<KetaEngine::PostEffectController>();
    timeScaleController_->Init();
    postEffectController_->Init();
}

void AttackEffect::Update() {
    timeScaleController_->Update(KetaEngine::Frame::DeltaTime());
    postEffectController_->Update(KetaEngine::Frame::DeltaTime());
}

void AttackEffect::PlayHitStop(const std::string& timeScaleName) {
    timeScaleController_->PlayTimeScale(timeScaleName);
}

void AttackEffect::PlayPostEffect(const std::string& effectName) {
    postEffectController_->PlayPostEffect(effectName);
}

void AttackEffect::EditorUpdate() {
    timeScaleController_->EditorUpdate();
    postEffectController_->EditorUpdate();
 }