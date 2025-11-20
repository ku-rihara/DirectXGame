#include "ObjEaseAnimationPlayer.h"

void ObjEaseAnimationPlayer::Init() {
    animationData_ = std::make_unique<ObjEaseAnimationData>();
}

void ObjEaseAnimationPlayer::Update() {
    if (animationData_) {
        animationData_->Update();
    }
}

void ObjEaseAnimationPlayer::Play(const std::string& categoryName, const std::string& animationName) {

    animationData_->Play();
   
    animationData_.reset();
    animationData_ = std::make_unique<ObjEaseAnimationData>();

    // 新しいアニメーションをロード
    animationData_->Init(animationName, categoryName);
    animationData_->LoadData();
    animationData_->Play();

    currentCategoryName_  = categoryName;
    currentAnimationName_ = animationName;
}

void ObjEaseAnimationPlayer::Stop() {
    if (animationData_) {
        animationData_->Stop();
    }
}

 Vector3 ObjEaseAnimationPlayer::GetCurrentScale() const {
    return animationData_->GetActiveKeyFrameValue(ObjEaseAnimationData::TransformType::Scale);
}
 Vector3 ObjEaseAnimationPlayer::GetCurrentRotation() const {
    return animationData_->GetActiveKeyFrameValue(ObjEaseAnimationData::TransformType::Rotation);
}

 Vector3 ObjEaseAnimationPlayer::GetCurrentTranslation() const {
    return animationData_->GetActiveKeyFrameValue(ObjEaseAnimationData::TransformType::Translation);
}