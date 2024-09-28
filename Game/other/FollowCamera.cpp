#include "FollowCamera.h"
#include "Easing.h"
#include "Matrix4x4.h"
#include "Input.h"
#include"MathFunction.h"
#include<cmath>
//class
#include"LockOn.h"

void FollowCamera::Init() { viewprojection_.Init(); }

void FollowCamera::Update() {
	XINPUT_STATE joyState;

	if (lockOn_&&lockOn_->GetEnemyTarget()) {
		Vector3 differectialVector = lockOn_->GetTargetPosition() - viewprojection_.translation_;
		differectialVector = Normalize(differectialVector);
		// Y軸周り角度(θy)
		viewprojection_.rotation_.y = std::atan2(differectialVector.x, differectialVector.z);
		
	} else {
		// 旋回操作
		if (Input::GetInstance()->GetJoystickState(0, joyState)) {
			const float rotateSpeed = 0.1f; // 回転速度
			destinationAngleY_ += (float)(joyState.Gamepad.sThumbRX / SHRT_MAX) * rotateSpeed;
			// 右スティック押し込みでリセット
			if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) {
				Reset();
			}
		}
	}
	viewprojection_.rotation_.y =LerpShortAngle(viewprojection_.rotation_.y,destinationAngleY_,0.15f);

	// 追従対象がいれば
	if (target_) {
		// 追従座標の補間
		interTarget_ = Lerp(interTarget_, target_->translation_, 0.4f);

		Vector3 offset = OffsetCalc();
		// 座標をコピーしてオフセットはずらす
		viewprojection_.translation_ = interTarget_ + offset;
	}
	// ビュー行列の更新
	viewprojection_.UpdateMatrix();
}

void FollowCamera::Reset() {
	// 追従対象がいれば
	if (target_) {
		// 追従座標・角度の初期化
		interTarget_ = target_->translation_;
		viewprojection_.rotation_.y = LerpShortAngle(viewprojection_.rotation_.y, target_->rotation_.y, 0.2f);
	}
	destinationAngleY_ = viewprojection_.rotation_.y;
	// 追従対象からのオフセット
	Vector3 offset = OffsetCalc();
	viewprojection_.translation_ = interTarget_ + offset;
}
Vector3 FollowCamera::OffsetCalc() const {
	// 追従対象からカメラまでのオフセット
	Vector3 offset = {0.0f, 2.0f, -10.0f};
	// カメラの角度から回転行列を計算する
	Matrix4x4 rotateMatrix = MakeRotateYMatrix(viewprojection_.rotation_.y);
	offset = TransformNormal(offset, rotateMatrix);
	return offset;
}

void FollowCamera::SetTarget(const WorldTransform* target) {
	target_ = target;
	Reset();
}