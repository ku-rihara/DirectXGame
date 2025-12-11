#include "ObjEaseAnimationPlayer.h"

void ObjEaseAnimationPlayer::Init() {
    BaseEffectPlayer::Init();
    // 初期オフセット値をリセット
    currentOffsetScale_       = Vector3::OneVector();
    currentOffsetRotation_    = Vector3::ZeroVector();
    currentOffsetTranslation_ = Vector3::ZeroVector();
}

void ObjEaseAnimationPlayer::Update(float speedRate) {
    if (effectData_) {
        effectData_->Update(speedRate);

        // 現在のアニメーション値を保存
        auto* animData = dynamic_cast<ObjEaseAnimationData*>(effectData_.get());
        if (animData) {
            currentOffsetScale_       = animData->GetActiveKeyFrameValue(ObjEaseAnimationData::TransformType::Scale);
            currentOffsetRotation_    = animData->GetActiveKeyFrameValue(ObjEaseAnimationData::TransformType::Rotation);
            currentOffsetTranslation_ = animData->GetActiveKeyFrameValue(ObjEaseAnimationData::TransformType::Translation);
        }
    }
}

void ObjEaseAnimationPlayer::Play(const std::string& animationName) {
    // 現在のオフセット値を保存
    SaveCurrentOffsets();

    if (effectData_) {
        effectData_->Pause();
    }

    effectData_.reset();
    effectData_ = CreateEffectData();

    auto* animeData = dynamic_cast<ObjEaseAnimationData*>(effectData_.get());
    if (animeData) {
        animeData->Init(animationName);
        animeData->LoadData();

        // 保存したオフセット値を初期値として設定
        animeData->SetOriginalValues(currentOffsetScale_, currentOffsetRotation_, currentOffsetTranslation_);

        animeData->Play();
    }

    currentEffectName_ = animationName;
    currentCategoryName_.clear();
}

void ObjEaseAnimationPlayer::PlayInCategory(const std::string& categoryName, const std::string& animationName) {
    // 現在のオフセット値を保存
    SaveCurrentOffsets();

    if (effectData_) {
        effectData_->Pause();
    }

    effectData_.reset();
    effectData_ = CreateEffectData();

    auto* animData = dynamic_cast<ObjEaseAnimationData*>(effectData_.get());
    if (animData) {
        animData->InitWithCategory(animationName, categoryName);
        animData->LoadData();

        // 保存したオフセット値を初期値として設定
        animData->SetOriginalValues(currentOffsetScale_, currentOffsetRotation_, currentOffsetTranslation_);

        animData->Play();
    }

    currentCategoryName_ = categoryName;
    currentEffectName_   = animationName;
}

void ObjEaseAnimationPlayer::SaveCurrentOffsets() {
    if (effectData_) {
        auto* animData = dynamic_cast<ObjEaseAnimationData*>(effectData_.get());
        if (animData && animData->IsPlaying()) {
            // 再生中の場合、現在の値を保存
            currentOffsetScale_       = animData->GetActiveKeyFrameValue(ObjEaseAnimationData::TransformType::Scale);
            currentOffsetRotation_    = animData->GetActiveKeyFrameValue(ObjEaseAnimationData::TransformType::Rotation);
            currentOffsetTranslation_ = animData->GetActiveKeyFrameValue(ObjEaseAnimationData::TransformType::Translation);
        }
    }
}

void ObjEaseAnimationPlayer::ResetOffsets() {
    currentOffsetScale_       = Vector3::OneVector();
    currentOffsetRotation_    = Vector3::ZeroVector();
    currentOffsetTranslation_ = Vector3::ZeroVector();
}

std::unique_ptr<BaseEffectData> ObjEaseAnimationPlayer::CreateEffectData() {
    return std::make_unique<ObjEaseAnimationData>();
}

Vector3 ObjEaseAnimationPlayer::GetCurrentScale() const {
    auto* animData = dynamic_cast<ObjEaseAnimationData*>(effectData_.get());
    if (animData) {
        return animData->GetActiveKeyFrameValue(ObjEaseAnimationData::TransformType::Scale);
    }
    return currentOffsetScale_;
}

Vector3 ObjEaseAnimationPlayer::GetCurrentRotation() const {
    auto* animData = dynamic_cast<ObjEaseAnimationData*>(effectData_.get());
    if (animData) {
        return animData->GetActiveKeyFrameValue(ObjEaseAnimationData::TransformType::Rotation);
    }
    return currentOffsetRotation_;
}

Vector3 ObjEaseAnimationPlayer::GetCurrentTranslation() const {
    auto* animData = dynamic_cast<ObjEaseAnimationData*>(effectData_.get());
    if (animData) {
        return animData->GetActiveKeyFrameValue(ObjEaseAnimationData::TransformType::Translation);
    }
    return currentOffsetTranslation_;
}

ObjEaseAnimationData* ObjEaseAnimationPlayer::GetAnimationData() {
    return dynamic_cast<ObjEaseAnimationData*>(effectData_.get());
}