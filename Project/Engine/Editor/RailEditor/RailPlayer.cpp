#include "RailPlayer.h"

void RailPlayer::Init() {
    BaseEffectPlayer::Init();
    currentPosition_ = Vector3::ZeroVector();
}

void RailPlayer::Update(const float& speedRate) {
    if (effectData_) {
        effectData_->Update(speedRate);

        if (RailData* railData = dynamic_cast<RailData*>(effectData_.get())) {
            currentPosition_ = railData->GetCurrentPosition();
        }
    }
}

void RailPlayer::UpdateWithDirection(const float& speedRate, const RailData::PositionMode& mode, const Vector3& direction) {
    if (RailData* railData = dynamic_cast<RailData*>(effectData_.get())) {
        railData->UpdateWithDirection(speedRate, mode, direction);
        currentPosition_ = railData->GetCurrentPosition();
    }
}

void RailPlayer::Play(const std::string& railName) {
    currentEffectName_ = railName;

    effectData_.reset();
    effectData_ = CreateEffectData();

    effectData_->Init(railName);
    effectData_->LoadData();
    effectData_->Play();
}

void RailPlayer::SetParent(WorldTransform* parent) {
    if (RailData* railData = dynamic_cast<RailData*>(effectData_.get())) {
        railData->SetParent(parent);
    }
}

std::unique_ptr<BaseEffectData> RailPlayer::CreateEffectData() {
    return std::make_unique<RailData>();
}