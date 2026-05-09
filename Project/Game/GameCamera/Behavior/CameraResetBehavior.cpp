#include "CameraResetBehavior.h"
#include "CameraFollowBehavior.h"
#include "GameCamera/GameCamera.h"
#include "MathFunction.h"
#include <cmath>
#include <numbers>

CameraResetBehavior::CameraResetBehavior(GameCamera* owner)
    : BaseCameraBehavior("CameraReset", owner) {

    // 初期フェーズをStartに設定
    phase_ = [this]() {
        Init();
    };
}

void CameraResetBehavior::Update(float time) {
    time; // 未使用

    if (phase_) {
        phase_();
    }
}

void CameraResetBehavior::Init() {

    // 目標Y軸回転角を設定するフェーズに移行
    phase_ = [this]() {
        SetTargetRotate();
    };
}

void CameraResetBehavior::SetTargetRotate() {
    // 追従対象のY軸回転角を取得
    targetRotationY_ = pOwner_->GetTarget()->rotation_.y;

    phase_ = [this]() {
        Reset();
    };
}

void CameraResetBehavior::Reset() {

    // Y軸回転を追従対象の角度に向けて補間
    pOwner_->RotateYInterpolation(targetRotationY_);
    pOwner_->SetDestinationAngleY(pOwner_->GetViewProjection().rotation_.y);

    // 位置を追従対象に向けて補間
    pOwner_->TranslateAdapt();

    // リセットが完了したか確認
    float currentCameraRotationY = pOwner_->GetViewProjection().rotation_.y;
    float rotationDifference     = std::abs(currentCameraRotationY - targetRotationY_);

    // 角度差を[-π, π]の範囲に正規化
    const float pi = std::numbers::pi_v<float>;
    // 180度以上の差分がある場合は、回転方向を考慮して差分を修正
    while (rotationDifference > pi) {
        rotationDifference -= ToRadian(360.0f);
    }

    // 差分の絶対値を取る
    rotationDifference = std::abs(rotationDifference);

    // 許容範囲
    const float PermissibleOffset = 2.0f;
    const float threshold         = ToRadian(PermissibleOffset); // 2度以内の差分が許容

    if (rotationDifference < threshold) {
        // リセット完了後、追従Behaviorに戻る
        pOwner_->ChangeBehavior(std::make_unique<CameraFollowBehavior>(pOwner_));
        return;
    }
}

void CameraResetBehavior::Debug() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    // デバッグ表示が必要な場合はここに実装
#endif
}