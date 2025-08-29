#include "DissolvePlayer.h"

void DissolvePlayer::Init() {
    dissolveData_ = std::make_unique<DissolveData>();
}

void DissolvePlayer::Update(float deltaTime) {
    if (dissolveData_) {
        dissolveData_->Update(deltaTime);
    }
}

void DissolvePlayer::Play(const std::string& dissolveName) {
    if (dissolveData_) {
        dissolveData_->Init(dissolveName);
        dissolveData_->LoadData();
        dissolveData_->Play();
    }
}

void DissolvePlayer::StopDissolve() {
    if (dissolveData_) {
        dissolveData_->Stop();
    }
}