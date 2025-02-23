#include "GameCamera.h"
//Function
#include"MathFunction.h"
//math
#include "Matrix4x4.h"
//input
#include"JoyState/JoyState.h"
//class
#include"LockOn/LockOn.h"
// behavior
#include"Behavior/GameCameraRoot.h"
#include"Behavior/GameCameraZoomInOut.h"

#include"ShakeState/ShakeStateMove.h"
#include"ShakeState/ShakeStateRoot.h"
/// std
#include<numbers>
#include<imgui.h>


void GameCamera::Init() {
	viewprojection_.Init();

	///* グローバルパラメータ
	globalParameter_ = GlobalParameter::GetInstance();
	globalParameter_->CreateGroup(groupName_, false);
	AddParmGroup();
	ApplyGlobalParameter();

	/*paramater_.firstOffset_ = { 5,13,-29.0f };
	paramater_.zoomOffset_  = { 0.0f,4.795f,-16.0f };
	paramater_.firstRotate_ = 14.25f;
	paramater_.zoomRotate_ = 5.7f;*/


	rotate_ = paramater_.firstRotate_;
	offset_ = paramater_.firstOffset_;

	ChangeBehavior(std::make_unique<GameCameraRoot>(this));
	ChangeShakeState(std::make_unique<ShakeStateRoot>(this));
}

void GameCamera::Update() {
	behavior_->Update();
	shakeState_->Update();
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
	Vector3 resultOffset = TransformNormal(offset + shakePos_, rotateMatrix);
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

Vector3 GameCamera::GetWorldPos()const {

	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得
	worldPos.x = viewprojection_.matView_.m[3][0];
	worldPos.y = viewprojection_.matView_.m[3][1];
	worldPos.z = viewprojection_.matView_.m[3][2];

	return worldPos;
}

void GameCamera::Debug() {
	ImGui::DragFloat("rotate", &rotate_, 0.01f);
	ImGui::DragFloat3("offset", &offset_.x,0.1f);
}
Vector3 GameCamera::GetTargetPos() const
{
	return Vector3();
}

void GameCamera::ChangeBehavior(std::unique_ptr<BaseGameCameraBehavior>behavior) {
	//引数で受け取った状態を次の状態としてセット
	behavior_ = std::move(behavior);
}

void  GameCamera::ChangeShakeState(std::unique_ptr<BaseShakeState>behavior) {
	//引数で受け取った状態を次の状態としてセット
	shakeState_ = std::move(behavior);
}

void GameCamera::ChangeShakeMode() {

	if (!dynamic_cast<ShakeStateRoot*>(shakeState_.get()))return;
	ChangeShakeState(std::make_unique<ShakeStateMove>(this));
}

void GameCamera::ChangeZoomInOut() {
	if (!dynamic_cast<GameCameraRoot*>(behavior_.get()))return;
	ChangeBehavior(std::make_unique<GameCameraZoomInOut>(this));
}

///=====================================================
///  ロード
///===================================================== 
void GameCamera::ParmLoadForImGui() {

	// ロードボタン
	if (ImGui::Button(std::format("Load {}", groupName_).c_str())) {

		globalParameter_->LoadFile(groupName_);
		// セーブ完了メッセージ
		ImGui::Text("Load Successful: %s", groupName_.c_str());
		ApplyGlobalParameter();
	}
}

///=====================================================
///  パラメータをグループに追加
///===================================================== 
void GameCamera::AddParmGroup() {
	globalParameter_->AddItem(groupName_, "zoomOffset_", paramater_.zoomOffset_);
	globalParameter_->AddItem(groupName_, "firstRotate_", paramater_.firstRotate_);
	globalParameter_->AddItem(groupName_, "firstOffset_", paramater_.firstOffset_);
	globalParameter_->AddItem(groupName_, "zoomORotate_", paramater_.zoomRotate_);
	globalParameter_->AddItem(groupName_, "backLashOffset_", paramater_.backLashOffset_);
	globalParameter_->AddItem(groupName_, "backLashTime_", paramater_.backLashTime_);
	globalParameter_->AddItem(groupName_, "backLashRatio_", paramater_.backLashRatio_);

}
///=====================================================
/// パラメータ値セット
///===================================================== 
void GameCamera::SetValues() {
	globalParameter_->SetValue(groupName_, "zoomOffset_", paramater_.zoomOffset_);
	globalParameter_->SetValue(groupName_, "firstRotate_", paramater_.firstRotate_);
	globalParameter_->SetValue(groupName_, "firstOffset_", paramater_.firstOffset_);
	globalParameter_->SetValue(groupName_, "zoomORotate_", paramater_.zoomRotate_);
	globalParameter_->SetValue(groupName_, "backLashOffset_", paramater_.backLashOffset_);
	globalParameter_->SetValue(groupName_, "backLashTime_", paramater_.backLashTime_);
	globalParameter_->SetValue(groupName_, "backLashRatio_", paramater_.backLashRatio_);

}


///=====================================================
///  ImGuiからパラメータを得る
///===================================================== 
void GameCamera::ApplyGlobalParameter() {
	// paramCombies_ から値を取得
	paramater_.zoomOffset_ = globalParameter_->GetValue<Vector3>(groupName_, "zoomOffset_");
	paramater_.firstOffset_ = globalParameter_->GetValue<Vector3>(groupName_, "firstOffset_");
	paramater_.firstRotate_ = globalParameter_->GetValue<float>(groupName_, "firstRotate_");
	paramater_.zoomRotate_ = globalParameter_->GetValue<float>(groupName_, "zoomORotate_");
	paramater_.backLashOffset_ = globalParameter_->GetValue<Vector3>(groupName_, "backLashOffset_");
	paramater_.backLashTime_ = globalParameter_->GetValue<float>(groupName_, "backLashTime_");
	paramater_.backLashRatio_ = globalParameter_->GetValue<float>(groupName_, "backLashRatio_");

}

///=========================================================
/// パラメータ調整
///==========================================================
void GameCamera::AdjustParm() {
	SetValues();
#ifdef _DEBUG

	if (ImGui::CollapsingHeader(groupName_.c_str())) {
		ImGui::PushID(groupName_.c_str());

		/// 変数の調整
		ImGui::DragFloat3("Zoom Offset", &paramater_.zoomOffset_.x, 0.1f);
		ImGui::DragFloat3("First Offset", &paramater_.firstOffset_.x, 0.1f);
		ImGui::SliderAngle("First Rotate", &paramater_.firstRotate_,0,1000);
		ImGui::SliderAngle("Zoom Rotate", &paramater_.zoomRotate_,0,1000);
		ImGui::DragFloat("Backlash Time", &paramater_.backLashTime_, 0.01f);
		ImGui::DragFloat("backLash Ratio", &paramater_.backLashRatio_, 0.01f);
		ImGui::DragFloat3("Backlash Offset", &paramater_.backLashOffset_.x, 0.1f);


		/// セーブとロード
		globalParameter_->ParmSaveForImGui(groupName_);
		ParmLoadForImGui();
		ImGui::PopID();
	}

#endif // _DEBUG
}

