#include "GameCamera.h"
//Function
#include"MathFunction.h"
//math
#include "Matrix4x4.h"
//input
#include"JoyState/JoyState.h"
//class
#include"LockOn/LockOn.h"
/// std
#include<numbers>


void GameCamera::Init() {
	viewprojection_.Init();
}

void GameCamera::Update() {


	// 旋回操作
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		const float rotateSpeed = 0.08f; // 回転速度

		// スティックの入力を正規化して回転角度を決定
		stickInput_ = { (float)joyState.Gamepad.sThumbRX / SHRT_MAX, (float)joyState.Gamepad.sThumbRY / SHRT_MAX, 0.0f };
		stickInput_ = Vector3::Normalize(stickInput_);

		// Y軸の回転角度を計算
		destinationAngleY_ += stickInput_.x * rotateSpeed;

		// 右スティック押し込みでリセット
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) {
			Reset();
		}
	}

	viewprojection_.rotation_.y = LerpShortAngle(viewprojection_.rotation_.y, destinationAngleY_, 0.3f);

	// 追従対象がいれば
	if (target_) {
		// 追従座標の補間
		interTarget_ = Lerp(interTarget_, target_->translation_, 0.4f);

		Vector3 offset = OffsetCalc();
		// 座標をコピーしてオフセットはずらす
		viewprojection_.translation_ = interTarget_ + offset;
	}
	// カメラの回転を固定の見下ろし角度に設定
	const float fixedPitchAngle = 10.0f * (std::numbers::pi_v<float>) / 180.0f; // 見下ろし角度をラジアンに変換
	viewprojection_.rotation_.x = fixedPitchAngle;       // 固定角度を設定
	// ビュー行列の更新
	viewprojection_.UpdateMatrix();
}

void GameCamera::Reset() {
	// 追従対象がいれば
	if (target_) {
		// 追従座標・角度の初期化
		interTarget_ = target_->translation_;
		viewprojection_.rotation_.y = LerpShortAngle(viewprojection_.rotation_.y, target_->rotation_.y, 0.3f);
	}
	destinationAngleY_ = viewprojection_.rotation_.y;
	// 追従対象からのオフセット
	Vector3 offset = OffsetCalc();
	viewprojection_.translation_ = interTarget_ + offset;
}

Vector3 GameCamera::OffsetCalc() const {
	// 追従対象からカメラまでのオフセット
	Vector3 offset = { 0.0f, 8.0f, -18.0f };
	// カメラの角度から回転行列を計算する
	Matrix4x4 rotateMatrix = MakeRotateYMatrix(viewprojection_.rotation_.y);
	offset = TransformNormal(offset, rotateMatrix);
	return offset;
}

void GameCamera::SetTarget(const WorldTransform* target) {
	target_ = target;
	Reset();
}

void GameCamera::GetIsCameraMove() {
	if (Vector3::Length(stickInput_) > 0.1f) {
		viewMoveTime_++;
		if (viewMoveTime_ >= kViewMoveTime_) {
		}
	}
}

Vector3 GameCamera::GetBaseCenterPosition()const {


	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得
	worldPos.x = viewprojection_.matView_.m[3][0];
	worldPos.y = viewprojection_.matView_.m[3][1];
	worldPos.z = viewprojection_.matView_.m[3][2];

	return worldPos;

}