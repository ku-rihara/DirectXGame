#include "CameraAnimation.h"

void CameraAnimation::Init() {
    // 生成
    animationData_ = std::make_unique<CameraAnimationData>();

    // オフセット値を初期化
    ResetOffsetParam();
}

void CameraAnimation::Update(const float& speedRate) {
    // アニメーションデータの更新
    if (animationData_) {
        animationData_->Update(speedRate);
        ApplyOffsetToViewProjection();
    }
}

void CameraAnimation::Play(const std::string& animationName) {
    if (!animationData_) {
        return;
    }

    currentAnimationName_ = animationName;

    animationData_.reset();
    animationData_ = std::make_unique<CameraAnimationData>();

    // アニメーションデータの初期化とロード
    animationData_->Init(currentAnimationName_);
    animationData_->LoadData();

    // 現在の値を初期値として保存
    SaveInitialValues();

    animationData_->Play();
}

void CameraAnimation::Reset() {
    if (animationData_) {
        animationData_->Reset();
    }

    // オフセット値をリセット
    ResetOffsetParam();
}

void CameraAnimation::SaveInitialValues() {

    if (!pViewProjection_) {
        return;
    }

    SetStartParam();
}

void CameraAnimation::ApplyOffsetToViewProjection() {
    if (!animationData_ || !pViewProjection_) {
        return;
    }

    if (!isAdapt_) {
        return;
    }

    // アニメーションデータをViewProjectionに適用
    animationData_->ApplyToViewProjection(*pViewProjection_);
}

void CameraAnimation::SetViewProjection(ViewProjection* viewProjection) {
    pViewProjection_ = viewProjection;
}

void CameraAnimation::SetStartParam() {
    startParam_.position = pViewProjection_->positionOffset_;
    startParam_.rotation = pViewProjection_->rotationOffset_;
    startParam_.fov      = pViewProjection_->fovAngleY_;
}

void CameraAnimation::ResetOffsetParam() {
    currentOffsetPosition_ = {0.0f, 0.0f, 0.0f};
    currentOffsetRotation_ = {0.0f, 0.0f, 0.0f};
    currentOffsetFov_      = 0.0f;
}