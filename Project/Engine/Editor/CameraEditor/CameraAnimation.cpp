#include "CameraAnimation.h"

using namespace KetaEngine;

void CameraAnimation::Init() {

    BaseEffectPlayer::Init();
    // オフセット値を初期化
    ResetOffsetParam();
}

void CameraAnimation::Update(float speedRate) {
    // アニメーションデータの更新
    if (effectData_) {
        effectData_->Update(speedRate);
        ApplyOffsetToViewProjection();
    }
}

void CameraAnimation::Play(const std::string& animationName) {
    if (!effectData_) {
        return;
    }

    currentEffectName_ = animationName;

    effectData_.reset();
    effectData_ = std::make_unique<CameraAnimationData>();

    // アニメーションデータの初期化とロード
    effectData_->Init(currentEffectName_);
    effectData_->LoadData();

    // 現在の値を初期値として保存
    SaveInitialValues();

    effectData_->Play();
}

void CameraAnimation::Reset() {
    if (effectData_) {
        effectData_->Reset();
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
    if (!effectData_ || !pViewProjection_) {
        return;
    }

    if (!isAdapt_) {
        return;
    }

    // アニメーションデータをViewProjectionに適用
    if (CameraAnimationData* cameraData = dynamic_cast<CameraAnimationData*>(effectData_.get())) {
        cameraData->ApplyToViewProjection(*pViewProjection_);
    }
}

std::unique_ptr<BaseEffectData> CameraAnimation::CreateEffectData() {
    return std::make_unique<CameraAnimationData>();
}

void CameraAnimation::SetViewProjection(ViewProjection* viewProjection) {
    pViewProjection_ = viewProjection;
}

void CameraAnimation::SetStartParam() {
    startParam_.position = pViewProjection_->positionOffset_;
    startParam_.rotation = pViewProjection_->rotationOffset_;
    startParam_.fov      = pViewProjection_->fovAngleY_;

    if (CameraAnimationData* date = dynamic_cast<CameraAnimationData*>(effectData_.get())) {
        date->SetInitialValues(startParam_.position, startParam_.rotation, startParam_.fov);
    }
}

void CameraAnimation::ResetOffsetParam() {
    currentOffsetPosition_ = {0.0f, 0.0f, 0.0f};
    currentOffsetRotation_ = {0.0f, 0.0f, 0.0f};
    currentOffsetFov_      = 0.0f;
}
