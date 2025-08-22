#include "CameraAnimation.h"
#include "MathFunction.h"
#include <imgui.h>
#include <iostream>

void CameraAnimation::Init() {
    // 生成
    animationData_ = std::make_unique<CameraAnimationData>();

    // 一時的なViewProjectionの初期化
    tempViewProjection_.Init();

    // 初期状態に設定
    state_      = AnimationState::IDLE;
    resetTimer_ = 0.0f;

    // オフセット値を初期化
    currentOffsetPosition_ = {0.0f, 0.0f, 0.0f};
    currentOffsetRotation_ = {0.0f, 0.0f, 0.0f};
    currentOffsetFov_      = 0.0f;

    // ステップ関数をクリア
    currentStep_ = nullptr;
}

void CameraAnimation::Update(float deltaTime) {
    // 現在のステップ関数を実行
    if (currentStep_) {
        currentStep_(deltaTime);
    }

    // アニメーションデータの更新
    if (animationData_) {
        animationData_->Update(deltaTime);
    }
}

void CameraAnimation::Play(const std::string& animationName) {
    if (!animationData_) {
        return;
    }

    currentAnimationName_ = animationName;

    // ステップ1: ロードステップから開始
    state_       = AnimationState::LOADING;
    currentStep_ = [this](float deltaTime) {
        StepLoad(deltaTime);
    };
}

void CameraAnimation::Stop() {
    if (animationData_) {
        animationData_->Reset();
    }

    state_       = AnimationState::IDLE;
    currentStep_ = nullptr;

    // オフセット値をリセット
    currentOffsetPosition_ = {0.0f, 0.0f, 0.0f};
    currentOffsetRotation_ = {0.0f, 0.0f, 0.0f};
    currentOffsetFov_      = 0.0f;
}

void CameraAnimation::ForceReset() {
    Stop();
}

void CameraAnimation::StepLoad(float deltaTime) {
    // アニメーションデータの初期化とロード
    animationData_->Init(currentAnimationName_);
    animationData_->LoadData();

    // 現在の値を初期値として保存
    SaveInitialValues();

    // アニメーションデータをリセットして再生開始
    animationData_->Reset();
    animationData_->Play();

    // ステップ2: 再生ステップに移行
    state_       = AnimationState::PLAYING;
    currentStep_ = [this](float deltaTime) {
        StepPlay(deltaTime);
    };
}

void CameraAnimation::StepPlay(float deltaTime) {
    // アニメーションが終了したかチェック
    if (animationData_->IsFinished()) {
        //  リセットステップに移行
        state_       = AnimationState::RESETTING;
        resetTimer_  = 0.0f;
        currentStep_ = [this](float deltaTime) {
            StepReset(deltaTime);
        };
        return;
    }

    // アニメーションデータからオフセット値を計算
    animationData_->ApplyToViewProjection(tempViewProjection_);

    // 初期値からの差分をオフセットとして計算
    currentOffsetPosition_ = tempViewProjection_.translation_ - initialParam_.position;
    currentOffsetRotation_ = tempViewProjection_.rotation_ - initialParam_.rotation;
    currentOffsetFov_      = tempViewProjection_.fovAngleY_ - initialParam_.fov;
}

void CameraAnimation::StepReset(float deltaTime) {
    resetTimer_ += deltaTime;

    if (resetTimer_ >= resetDuration_) {
        // リセット完了
        currentOffsetPosition_ = {0.0f, 0.0f, 0.0f};
        currentOffsetRotation_ = {0.0f, 0.0f, 0.0f};
        currentOffsetFov_      = 0.0f;

        state_       = AnimationState::IDLE;
        currentStep_ = nullptr;
        return;
    }

    // リセット進行率を計算
    float progress = resetTimer_ / resetDuration_;

    // イージングを適用した進行率を計算
    float easedProgressPos = ApplyEasing(progress, static_cast<EasingType>(resetPosEaseType_));
    float easedProgressRot = ApplyEasing(progress, static_cast<EasingType>(resetRotateEaseType_));
    float easedProgressFov = ApplyEasing(progress, static_cast<EasingType>(resetFovEaseType_));

    // 現在のオフセット値から初期値（0,0,0）へ補間
    Vector3 startOffsetPos = tempViewProjection_.translation_ - initialParam_.position;
    Vector3 startOffsetRot = tempViewProjection_.rotation_ - initialParam_.rotation;
    float startOffsetFov   = tempViewProjection_.fovAngleY_ - initialParam_.fov;

    currentOffsetPosition_ = Lerp(startOffsetPos, {0.0f, 0.0f, 0.0f}, easedProgressPos);
    currentOffsetRotation_ = Lerp(startOffsetRot, {0.0f, 0.0f, 0.0f}, easedProgressRot);
    currentOffsetFov_      = Lerp(startOffsetFov, 0.0f, easedProgressFov);
}

void CameraAnimation::SaveInitialValues() {
    // 実際のViewProjectionから現在の値を取得
    // ここでは仮に現在のtempViewProjectionの値を使用
    // 実際の実装では、メインのViewProjectionから値を取得する必要があります
    initialParam_.position = tempViewProjection_.translation_;
    initialParam_.rotation = tempViewProjection_.rotation_;
    initialParam_.fov      = tempViewProjection_.fovAngleY_;
}

void CameraAnimation::ApplyOffsetToViewProjection() {
    if (!animationData_) {
        return;
    }

    // アニメーションデータを一時的なViewProjectionに適用
    animationData_->ApplyToViewProjection(tempViewProjection_);
}