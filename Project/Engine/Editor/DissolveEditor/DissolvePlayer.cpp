#include "DissolvePlayer.h"

using namespace KetaEngine;

void DissolvePlayer::Init() {
    BaseEffectPlayer::Init();
}

void DissolvePlayer::Update(float speedRate) {
    if (effectData_) {
        effectData_->Update(speedRate);
    }
}

void DissolvePlayer::Play(const std::string& dissolveName) {
    effectData_.reset();
    effectData_ = CreateEffectData();

    if (effectData_) {
        effectData_->Init(dissolveName);
        effectData_->LoadData();
        effectData_->Play();
        currentEffectName_ = dissolveName;
    }
}

void DissolvePlayer::ApplyToMaterial(ModelMaterial& material) {
    DissolveData* dissolveData = GetDissolveData();

    if (!dissolveData) {
        return;
    }

    // ディゾルブ値を適用
    material.SetDissolveThreshold(dissolveData->GetCurrentThreshold());
    material.SetEnableDissolve(dissolveData->IsDissolveEnabled());

    // テクスチャパスが設定されている場合は適用
    const std::string& texturePath = dissolveData->GetCurrentTexturePath();
    if (!texturePath.empty()) {
        material.SetDissolveNoizeTexture(texturePath);
    }
}

std::unique_ptr<BaseEffectData> DissolvePlayer::CreateEffectData() {
    return std::make_unique<DissolveData>();
}

DissolveData* DissolvePlayer::GetDissolveData() {
    return dynamic_cast<DissolveData*>(effectData_.get());
}