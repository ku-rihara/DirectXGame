#include "ShakePlayer.h"

void ShakePlayer::Init() {
    totalShakeOffset_ = {0.0f, 0.0f, 0.0f};
    shakeDataMap_.clear();
}

void ShakePlayer::Update(float deltaTime) {
    // 全てのシェイクを更新
    for (auto& pair : shakeDataMap_) {
        pair.second->Update(deltaTime);
    }

    // 終了したシェイクを削除
    for (auto it = shakeDataMap_.begin(); it != shakeDataMap_.end();) {
        if (it->second->IsFinished()) {
            it = shakeDataMap_.erase(it);
        } else {
            ++it;
        }
    }

    // 合成シェイクオフセットを計算
    UpdateTotalShakeOffset();
}

void ShakePlayer::UpdateTotalShakeOffset() {
    totalShakeOffset_ = {0.0f, 0.0f, 0.0f};

    // 全てのアクティブなシェイクのオフセットを合成
    for (const auto& pair : shakeDataMap_) {
        Vector3 shakeOffset = pair.second->GetShakeOffset();
        totalShakeOffset_.x += shakeOffset.x;
        totalShakeOffset_.y += shakeOffset.y;
        totalShakeOffset_.z += shakeOffset.z;
    }
}

void ShakePlayer::Play(const std::string& shakeName) {
    auto it = shakeDataMap_.find(shakeName);
    if (it != shakeDataMap_.end()) {
        it->second->LoadData();
        it->second->Play();
    } else {
     
        auto shakeData = std::make_unique<ShakeData>();
        shakeData->Init(shakeName);
        shakeData->LoadData(); // パラメータをロード
        shakeData->Play();
        shakeDataMap_[shakeName] = std::move(shakeData);
    }
}

void ShakePlayer::StopShake() {
    StopAllShakes();
}

void ShakePlayer::StopShake(const std::string& shakeName) {
    auto it = shakeDataMap_.find(shakeName);
    if (it != shakeDataMap_.end()) {
        it->second->Stop();
    }
}

void ShakePlayer::StopAllShakes() {
    for (auto& pair : shakeDataMap_) {
        pair.second->Stop();
    }
    totalShakeOffset_ = {0.0f, 0.0f, 0.0f};
}

bool ShakePlayer::IsPlaying(const std::string& shakeName) const {
    auto it = shakeDataMap_.find(shakeName);
    if (it != shakeDataMap_.end()) {
        return it->second->IsPlaying();
    }
    return false;
}

bool ShakePlayer::IsAnyShakePlaying() const {
    for (const auto& pair : shakeDataMap_) {
        if (pair.second->IsPlaying()) {
            return true;
        }
    }
    return false;
}