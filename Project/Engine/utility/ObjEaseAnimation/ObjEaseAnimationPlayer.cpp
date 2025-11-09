#include "ObjEaseAnimationPlayer.h"

void ObjEaseAnimationPlayer::Init() {
    animationData_ = std::make_unique<ObjEaseAnimationData>();
}

void ObjEaseAnimationPlayer::Update(const float& deltaTime) {
    if (animationData_) {
        animationData_->Update(deltaTime);
    }
}

void ObjEaseAnimationPlayer::Play(const std::string& categoryName, const std::string& animationName) {
    if (!animationData_) {
        animationData_ = std::make_unique<ObjEaseAnimationData>();
    }

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