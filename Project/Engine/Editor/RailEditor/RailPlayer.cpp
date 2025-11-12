#include "RailPlayer.h"

void RailPlayer::Init() {
    currentPosition_ = {0.0f, 0.0f, 0.0f};
    railData_        = std::make_unique<RailData>();
}

void RailPlayer::Update(const float& speed, const RailData::PositionMode& mode, const Vector3& direction) {
    if (!railData_) {
        return;
    }

    railData_->Update(speed, mode, direction);
    currentPosition_ = railData_->GetCurrentPosition();
}

void RailPlayer::Play(const std::string& railName) {

    railData_.reset();
    railData_ = std::make_unique<RailData>();

    railData_->Init(railName);
    railData_->LoadData();
    railData_->Play();
}

void RailPlayer::Stop() {
    if (railData_) {
        railData_->Stop();
        currentPosition_ = {0.0f, 0.0f, 0.0f};
    }
}

void RailPlayer::SetParent(WorldTransform* parent) {
    if (railData_) {
        railData_->SetParent(parent);
    }
}