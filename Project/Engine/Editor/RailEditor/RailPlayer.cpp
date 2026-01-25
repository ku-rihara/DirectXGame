#include "RailPlayer.h"

using namespace KetaEngine;

void RailPlayer::Init() {
    BaseEffectPlayer::Init();
    currentPosition_ = Vector3::ZeroVector();
}

void RailPlayer::Update(float speedRate) {
    if (effectData_) {
        effectData_->Update(speedRate);

        if (RailData* railData = dynamic_cast<RailData*>(effectData_.get())) {
            currentPosition_ = railData->GetCurrentPosition();
        }
    }
}

void RailPlayer::UpdateWithDirection(float speedRate, const RailData::PositionMode& mode, const Vector3& direction) {
    if (RailData* railData = dynamic_cast<RailData*>(effectData_.get())) {
        railData->UpdateWithDirection(speedRate, mode, direction);
        currentPosition_ = railData->GetCurrentPosition();
    }
}

void RailPlayer::Play(const std::string& railName, const std::string& categoryName) {
    if (effectData_) {
        effectData_->Pause();
    }

    effectData_.reset();
    effectData_ = CreateEffectData();

    currentCategoryName_ = categoryName;
    currentEffectName_   = railName;

    if (RailData* railData = dynamic_cast<RailData*>(effectData_.get())) {
        railData->Init(railName, categoryName);
        railData->LoadData();
        railData->Play();
    }
}

void RailPlayer::SetParent(WorldTransform* parent) {
    if (RailData* railData = dynamic_cast<RailData*>(effectData_.get())) {
        railData->SetParent(parent);
    }
}

std::unique_ptr<BaseEffectData> RailPlayer::CreateEffectData() {
    return std::make_unique<RailData>();
}