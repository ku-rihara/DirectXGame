#include "ShakePlayer.h"

void ShakePlayer::Init() {
    totalShakeOffset_ = {0.0f, 0.0f, 0.0f};
}

void ShakePlayer::Update(float deltaTime) {
    // 所有するシェイクデータを更新
    for (auto& [name, shakeData] : ownedShakeData_) {
        shakeData->Update(deltaTime);
    }

    // 合成シェイクオフセットを計算
    UpdateTotalShakeOffset();
}

void ShakePlayer::UpdateTotalShakeOffset() {
    totalShakeOffset_ = {0.0f, 0.0f, 0.0f};

    // 所有するシェイクのオフセットを合成
    for (const auto& [name, shakeData] : ownedShakeData_) {
        if (shakeData->IsPlaying()) {
            Vector3 offset = shakeData->GetShakeOffset();
            totalShakeOffset_.x += offset.x;
            totalShakeOffset_.y += offset.y;
            totalShakeOffset_.z += offset.z;
        }
    }

    // 登録されたシェイクのオフセットを合成
    for (const auto& [name, shakeData] : registeredShakeData_) {
        if (shakeData && shakeData->IsPlaying()) {
            Vector3 offset = shakeData->GetShakeOffset();
            totalShakeOffset_.x += offset.x;
            totalShakeOffset_.y += offset.y;
            totalShakeOffset_.z += offset.z;
        }
    }
}

void ShakePlayer::PlayShake(const std::string& shakeName) {
    // 所有するシェイクから検索
    auto ownedIt = ownedShakeData_.find(shakeName);
    if (ownedIt != ownedShakeData_.end()) {
        ownedIt->second->Play();
        return;
    }

    // 登録されたシェイクから検索
    auto registeredIt = registeredShakeData_.find(shakeName);
    if (registeredIt != registeredShakeData_.end() && registeredIt->second) {
        registeredIt->second->Play();
    }
}

void ShakePlayer::StopShake(const std::string& shakeName) {
    // 所有するシェイクから検索
    auto ownedIt = ownedShakeData_.find(shakeName);
    if (ownedIt != ownedShakeData_.end()) {
        ownedIt->second->Stop();
        return;
    }

    // 登録されたシェイクから検索
    auto registeredIt = registeredShakeData_.find(shakeName);
    if (registeredIt != registeredShakeData_.end() && registeredIt->second) {
        registeredIt->second->Stop();
    }
}

void ShakePlayer::StopAllShakes() {
    // 所有するシェイクをすべて停止
    for (auto& [name, shakeData] : ownedShakeData_) {
        shakeData->Stop();
    }

    // 登録されたシェイクをすべて停止
    for (auto& [name, shakeData] : registeredShakeData_) {
        if (shakeData) {
            shakeData->Stop();
        }
    }

    // オフセットをクリア
    totalShakeOffset_ = {0.0f, 0.0f, 0.0f};
}

void ShakePlayer::AddShakeData(std::unique_ptr<ShakeData> shakeData) {
    if (shakeData) {
        std::string name      = shakeData->GetGroupName();
        ownedShakeData_[name] = std::move(shakeData);
    }
}

void ShakePlayer::RegisterShakeData(const std::string& name, ShakeData* shakeData) {
    registeredShakeData_[name] = shakeData;
}

bool ShakePlayer::IsPlayingShake(const std::string& shakeName) const {
    // 所有するシェイクから検索
    auto ownedIt = ownedShakeData_.find(shakeName);
    if (ownedIt != ownedShakeData_.end()) {
        return ownedIt->second->IsPlaying();
    }

    // 登録されたシェイクから検索
    auto registeredIt = registeredShakeData_.find(shakeName);
    if (registeredIt != registeredShakeData_.end() && registeredIt->second) {
        return registeredIt->second->IsPlaying();
    }

    return false;
}

bool ShakePlayer::IsAnyShakePlaying() const {
    // 所有するシェイクをチェック
    for (const auto& [name, shakeData] : ownedShakeData_) {
        if (shakeData->IsPlaying()) {
            return true;
        }
    }

    // 登録されたシェイクをチェック
    for (const auto& [name, shakeData] : registeredShakeData_) {
        if (shakeData && shakeData->IsPlaying()) {
            return true;
        }
    }

    return false;
}