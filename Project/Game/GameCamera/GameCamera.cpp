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
#include<imgui.h>


void GameCamera::Init() {
	viewprojection_.Init();

	rotate_ = 5.6f;
	offset_ = { 0,11,-36.0f };
}

void GameCamera::Update() {

	MoveUpdate();
	// ビュー行列の更新
	viewprojection_.UpdateMatrix();
}

void GameCamera::MoveUpdate() {
	Input* input = Input::GetInstance();
	const float rotateSpeed = 0.08f; // 回転速度

	// 旋回操作: 左右キーでY軸の回転を操作
	stickInput_ = { 0.0f, 0.0f, 0.0f }; // スティック入力相当のベクトルをリセット


	///----------------------------------------------------------------------------------
	///kyebord
	///---------------------------------------------------------------------------------- 
	if (input->PushKey(DIK_RIGHT) || input->PushKey(DIK_LEFT)) {

		if (input->PushKey(DIK_RIGHT)) {
			stickInput_.x = 1.0f;
		}
		else if (input->PushKey(DIK_LEFT)) {
			stickInput_.x = -1.0f;
		}
	}
	else  if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		///---------------------------------------------------------------------------------- 
		///pad
		///----------------------------------------------------------------------------------  

		// スティックの入力を正規化して回転角度を決定
		stickInput_ = { (float)joyState.Gamepad.sThumbRX / SHRT_MAX, (float)joyState.Gamepad.sThumbRY / SHRT_MAX, 0.0f };

	}
	// tekiou 
	if ((stickInput_).Length() > 0.1f) {
		stickInput_ = (stickInput_).Normalize();
		destinationAngleY_ += stickInput_.x * rotateSpeed;
	}

	// リセット操作: Rキーでリセット
	if (input->TrrigerKey(DIK_R)) {
		Reset();
	}
	else {

		if (Input::GetInstance()->GetJoystickState(0, joyState)) {
			// 右スティック押し込みでリセット
			if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) {
				Reset();
			}
		}
	}

	// Y軸の回転補間処理
	viewprojection_.rotation_.y = LerpShortAngle(viewprojection_.rotation_.y, destinationAngleY_, 0.3f);

	TranslateAdapt(); // 変位適応

	RotateAdapt();    // 回転適応
}

void GameCamera::RotateAdapt() {
	// Y軸の回転補間処理
	viewprojection_.rotation_.y = LerpShortAngle(viewprojection_.rotation_.y, destinationAngleY_, 0.3f);

	// 見下ろし角度の固定
	const float fixedPitchAngle = rotate_ * std::numbers::pi_v<float> / 180.0f;
	viewprojection_.rotation_.x = fixedPitchAngle;
}

void GameCamera::TranslateAdapt() {
	if (!target_) return;
	interTarget_ = Lerp(interTarget_, target_->translation_, 1.0f);
	Vector3 offset = OffsetCalc(offset_);
	viewprojection_.translation_ = interTarget_ + offset;

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
	Vector3 offset = OffsetCalc(offset_);
	viewprojection_.translation_ = interTarget_ + offset;
}

Vector3 GameCamera::OffsetCalc(const Vector3& offset) const {
	
	// カメラの角度から回転行列を計算する
	Matrix4x4 rotateMatrix = MakeRotateYMatrix(viewprojection_.rotation_.y);
	Vector3 resultOffset = TransformNormal(offset, rotateMatrix);
	return resultOffset;
}

void GameCamera::SetTarget(const WorldTransform* target) {
	target_ = target;
	Reset();
}

void GameCamera::GetIsCameraMove() {
	if ((stickInput_).Length() > 0.1f) {

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

void GameCamera::Debug() {
	ImGui::DragFloat("rotate", & rotate_, 0.01f);
	ImGui::DragFloat3("offset", &offset_.x, 0.1f);
}