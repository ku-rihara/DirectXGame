#include "ObjEaseAnimationPlayer.h"

void ObjEaseAnimationPlayer::Init() {

     BaseEffectPlayer::Init();
}

void ObjEaseAnimationPlayer::Update(const float& speedRate) {
    if (effectData_) {
        effectData_->Update(speedRate);
    }
}

void ObjEaseAnimationPlayer::PlayInCategory(const std::string& categoryName, const std::string& animationName) {
    if (effectData_) {
        effectData_->Pause();
    }

    effectData_.reset();
    effectData_ = CreateEffectData();

    // キャスト
    auto* animData = dynamic_cast<ObjEaseAnimationData*>(effectData_.get());
    if (animData) {
        animData->InitWithCategory(animationName, categoryName);
        animData->LoadData();
        animData->Play();
    }

    currentCategoryName_ = categoryName;
    currentEffectName_   = animationName;
}

std::unique_ptr<BaseEffectData> ObjEaseAnimationPlayer::CreateEffectData() {
    return std::make_unique<ObjEaseAnimationData>();
}

Vector3 ObjEaseAnimationPlayer::GetCurrentScale() const {
    auto* animData = dynamic_cast<ObjEaseAnimationData*>(effectData_.get());
    if (animData) {
        return animData->GetActiveKeyFrameValue(ObjEaseAnimationData::TransformType::Scale);
    }
    return Vector3::OneVector();
}

Vector3 ObjEaseAnimationPlayer::GetCurrentRotation() const {
    auto* animData = dynamic_cast<ObjEaseAnimationData*>(effectData_.get());
    if (animData) {
        return animData->GetActiveKeyFrameValue(ObjEaseAnimationData::TransformType::Rotation);
    }
    return Vector3::ZeroVector();
}

Vector3 ObjEaseAnimationPlayer::GetCurrentTranslation() const {
    auto* animData = dynamic_cast<ObjEaseAnimationData*>(effectData_.get());
    if (animData) {
        return animData->GetActiveKeyFrameValue(ObjEaseAnimationData::TransformType::Translation);
    }
    return Vector3::ZeroVector();
}

ObjEaseAnimationData* ObjEaseAnimationPlayer::GetAnimationData() {
    return dynamic_cast<ObjEaseAnimationData*>(effectData_.get());
}