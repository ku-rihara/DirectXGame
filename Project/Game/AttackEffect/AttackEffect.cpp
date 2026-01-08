#include "AttackEffect.h"
#include"Frame/Frame.h" 

void AttackEffect::Init() {
    timeScalePlayer_      = std::make_unique<KetaEngine::TimeScalePlayer>();
    postEffectController_ = std::make_unique<KetaEngine::PostEffectController>();
    timeScalePlayer_->Init();
    postEffectController_->Init();
}

void AttackEffect::Update() {
    timeScalePlayer_->Update();
    postEffectController_->Update(KetaEngine::Frame::DeltaTime());
}

void AttackEffect::PlayHitStop(const std::string& timeScaleName) {
    timeScalePlayer_->Play(timeScaleName);
}

void AttackEffect::PlayPostEffect(const std::string& effectName) {
    postEffectController_->PlayPostEffect(effectName);
}

void AttackEffect::EditorUpdate() {
    postEffectController_->EditorUpdate();
 }