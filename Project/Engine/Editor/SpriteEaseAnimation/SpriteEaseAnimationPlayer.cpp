#include "SpriteEaseAnimationPlayer.h"

using namespace KetaEngine;

void SpriteEaseAnimationPlayer::Init() {
    BaseEffectPlayer::Init();
}

void SpriteEaseAnimationPlayer::Update(float speedRate) {
    if (effectData_) {
        effectData_->Update(speedRate);
    }
}

void SpriteEaseAnimationPlayer::Play(const std::string& animationName, const std::string& categoryName) {
    if (effectData_) {
        effectData_->Pause();
    }

    effectData_.reset();
    effectData_ = CreateEffectData();

    auto* animeData = dynamic_cast<SpriteEaseAnimationData*>(effectData_.get());
    if (animeData) {
        animeData->Init(animationName, categoryName);
        animeData->LoadData();
        animeData->Play();
    }

    currentCategoryName_ = categoryName;
    currentEffectName_   = animationName;
}

std::unique_ptr<BaseEffectData> SpriteEaseAnimationPlayer::CreateEffectData() {
    return std::make_unique<SpriteEaseAnimationData>();
}

Vector2 SpriteEaseAnimationPlayer::GetCurrentScale() const {
    auto* animeData = dynamic_cast<SpriteEaseAnimationData*>(effectData_.get());
    if (animeData) {
        return animeData->GetActiveScale();
    }
    return Vector2::OneVector();
}

Vector2 SpriteEaseAnimationPlayer::GetCurrentPosition() const {
    auto* animeData = dynamic_cast<SpriteEaseAnimationData*>(effectData_.get());
    if (animeData) {
        return animeData->GetActivePosition();
    }
    return Vector2::ZeroVector();
}

Vector3 SpriteEaseAnimationPlayer::GetCurrentRotation() const {
    auto* animeData = dynamic_cast<SpriteEaseAnimationData*>(effectData_.get());
    if (animeData) {
        return animeData->GetActiveRotation();
    }
    return Vector3::ZeroVector();
}

Vector3 SpriteEaseAnimationPlayer::GetCurrentColor() const {
    auto* animeData = dynamic_cast<SpriteEaseAnimationData*>(effectData_.get());
    if (animeData) {
        return animeData->GetActiveColor();
    }
    return {1.0f, 1.0f, 1.0f};
}

float SpriteEaseAnimationPlayer::GetCurrentAlpha() const {
    auto* animeData = dynamic_cast<SpriteEaseAnimationData*>(effectData_.get());
    if (animeData) {
        return animeData->GetActiveAlpha();
    }
    return 1.0f;
}

SpriteEaseAnimationData* SpriteEaseAnimationPlayer::GetAnimationData() {
    return dynamic_cast<SpriteEaseAnimationData*>(effectData_.get());
}
