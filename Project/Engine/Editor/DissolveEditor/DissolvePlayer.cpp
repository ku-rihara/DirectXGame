#include "DissolvePlayer.h"
#include "Base/Material/ModelMaterial.h"

using namespace KetaEngine;

void DissolvePlayer::Init() {
    BaseEffectPlayer::Init();
}

void DissolvePlayer::Update(float speedRate) {
    if (effectData_) {
        effectData_->Update(speedRate);
    }
}

void DissolvePlayer::Play(const std::string& dissolveName, const std::string& category) {
    effectData_.reset();
    effectData_ = CreateEffectData();

    if (effectData_) {
        effectData_->Init(dissolveName, category);
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
    material.GetMaterialData()->dissolveThreshold = dissolveData->GetCurrentThreshold();
    material.GetMaterialData()->enableDissolve    = dissolveData->IsDissolveEnabled();

    // テクスチャパスが設定されている場合は適用
    const std::string& texturePath = dissolveData->GetCurrentTexturePath();
    if (!texturePath.empty()) {
        material.SetDissolveNoizeTexture(texturePath);
    }
}

float DissolvePlayer::GetThreshold() const {
    const DissolveData* data = dynamic_cast<const DissolveData*>(effectData_.get());
    return data ? data->GetCurrentThreshold() : 1.0f;
}

bool DissolvePlayer::IsEnabled() const {
    const DissolveData* data = dynamic_cast<const DissolveData*>(effectData_.get());
    return data ? data->IsDissolveEnabled() : false;
}

const std::string& DissolvePlayer::GetTexturePath() const {
    static const std::string empty;
    const DissolveData* data = dynamic_cast<const DissolveData*>(effectData_.get());
    return data ? data->GetCurrentTexturePath() : empty;
}

float DissolvePlayer::GetStartThreshold() const {
    const DissolveData* data = dynamic_cast<const DissolveData*>(effectData_.get());
    return data ? data->GetStartThreshold() : 1.0f;
}

float DissolvePlayer::GetEndThreshold() const {
    const DissolveData* data = dynamic_cast<const DissolveData*>(effectData_.get());
    return data ? data->GetEndThreshold() : 0.0f;
}

float DissolvePlayer::GetMaxTime() const {
    const DissolveData* data = dynamic_cast<const DissolveData*>(effectData_.get());
    return data ? data->GetMaxTime() : 1.0f;
}

float DissolvePlayer::GetOffsetTime() const {
    const DissolveData* data = dynamic_cast<const DissolveData*>(effectData_.get());
    return data ? data->GetOffsetTime() : 0.0f;
}

int32_t DissolvePlayer::GetEaseType() const {
    const DissolveData* data = dynamic_cast<const DissolveData*>(effectData_.get());
    return data ? data->GetEaseType() : 0;
}

std::unique_ptr<BaseEffectData> DissolvePlayer::CreateEffectData() {
    return std::make_unique<DissolveData>();
}

DissolveData* DissolvePlayer::GetDissolveData() {
    return dynamic_cast<DissolveData*>(effectData_.get());
}