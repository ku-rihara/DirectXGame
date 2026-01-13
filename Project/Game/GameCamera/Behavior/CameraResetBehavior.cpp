#include "CameraResetBehavior.h"
#include "CameraFollowBehavior.h"
#include "GameCamera/GameCamera.h"
#include "MathFunction.h"
#include <cstdlib>

CameraResetBehavior::CameraResetBehavior(GameCamera* owner)
    : BaseCameraBehavior("CameraReset", owner) {}

void CameraResetBehavior::Update(float time) {
    time; // 未使用

    // Y軸回転を0に向けて補間
    float targetRotationY = pOwner_->GetTarget()->rotation_.y;

    pOwner_->RotateYInterpolation(targetRotationY);
    pOwner_->SetDestinationAngleY(pOwner_->GetViewProjection().rotation_.y);

    // 位置を追従対象に向けて補間
    pOwner_->TranslateAdapt();

    // リセットが完了したか確認
    const float degreeThreshold = ToDegree(pOwner_->GetTarget()->rotation_.y);
    const float threshold       = ToRadian(degreeThreshold + 2.0f); // 2度以内

    if (std::abs(pOwner_->GetViewProjection().rotation_.y) < threshold) {

        // リセット完了後、追従Behaviorに戻る
        pOwner_->ChangeBehavior(std::make_unique<CameraFollowBehavior>(pOwner_));
        return;
    }
}

void CameraResetBehavior::Debug() {
#ifdef _DEBUG
    // デバッグ表示が必要な場合はここに実装
#endif
}