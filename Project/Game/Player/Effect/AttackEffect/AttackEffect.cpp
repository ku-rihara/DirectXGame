#include "AttackEffect.h"
#include"Frame/Frame.h" 

void AttackEffect::Init() {
    timeScalePlayer_ = std::make_unique<KetaEngine::TimeScalePlayer>();
    postEffectEditor_ = std::make_unique<KetaEngine::PostEffectEditor>();
    timeScalePlayer_->Init();
    postEffectEditor_->Init("PostEffect");
}

void AttackEffect::Update() {
    timeScalePlayer_->Update(KetaEngine::Frame::DeltaTime());
    postEffectEditor_->Update(KetaEngine::Frame::DeltaTime());
}

void AttackEffect::PlayHitStop(const std::string& timeScaleName) {
    timeScalePlayer_->Play(timeScaleName);
}

void AttackEffect::PlayPostEffect(const std::string& effectName) {
    postEffectEditor_->PlayPostEffect(effectName);
}

void AttackEffect::EditorUpdate() {
    postEffectEditor_->EditorUpdate();
}