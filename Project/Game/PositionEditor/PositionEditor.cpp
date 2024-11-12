#include"PositionEditor.h"
/// input
#include"input/Input.h"
#include<imgui.h>

void PositionEditor::Init() {

	obj3d_.reset(Object3d::CreateModel("cube", ".obj"));
	transform_.Init();
}

void PositionEditor::Update(const ViewProjection& viewProjection) {
	//可視化オブジェクトに代入
	transform_.translation_ = Input::GetMousePos3D(viewProjection, mouseDepth_);

	transform_.UpdateMatrix();

	/// ブロック
	if (Input::GetInstance()->TrrigerKey(DIK_0)) {
		mode_ = NONE;
	}
	if (Input::GetInstance()->TrrigerKey(DIK_1)) {
		mode_ = SLOW;
	}
	if (Input::GetInstance()->TrrigerKey(DIK_2)) {
		mode_ = FAST;
	}
	if (Input::GetInstance()->TrrigerKey(DIK_3)) {
		mode_ =STOP ;
	}
	if (Input::GetInstance()->TrrigerKey(DIK_4)) {
		mode_ = NORMAL;
	}

	/// 敵
	if (Input::GetInstance()->TrrigerKey(DIK_N)) {
		enemyType_ = NORMALE;
	}
	if (Input::GetInstance()->TrrigerKey(DIK_S)) {
		enemyType_ = SIDE;
	}
	if (Input::GetInstance()->TrrigerKey(DIK_B)) {
		enemyType_ = BOSS;
	}
}


void PositionEditor::PutControlPoint(ControlPointManager* controlPointManager) {
	if (mode_ == NONE) {
		//制御点追加
		if (Input::GetInstance()->IsTriggerMouse(3)) {
			controlPointManager->AddControlPoint(transform_.translation_);
		}
	}
}

void PositionEditor::PutEnemy(EnemyManager* enemyManager) {
	if (enemyType_ == NORMALE) {
		if (Input::GetInstance()->IsTriggerMouse(4)) {
			enemyManager->AddNormalEnemy(transform_.translation_);
		}
	}

	if (enemyType_ == SIDE) {
		if (Input::GetInstance()->IsTriggerMouse(4)) {
			enemyManager->AddSideEnemy(transform_.translation_);
		}
	}
	if (enemyType_ == BOSS) {
		if (Input::GetInstance()->IsTriggerMouse(4)) {
			enemyManager->AddBoss(transform_.translation_);
		}
	}
}

void PositionEditor::PutSpeedPoint(SpeedPointManager* speedPointManager) {
	if (mode_ == SLOW) {
		//制御点追加
		if (Input::GetInstance()->IsTriggerMouse(3)) {
			speedPointManager->AddSlowSpeed(transform_.translation_);
		}
	}

	if (mode_ == FAST) {
		//制御点追加
		if (Input::GetInstance()->IsTriggerMouse(3)) {
			speedPointManager->AddFastSpeed(transform_.translation_);
		}
	}

	if (mode_ == STOP) {
		//制御点追加
		if (Input::GetInstance()->IsTriggerMouse(3)) {
			speedPointManager->AddStopSpeed(transform_.translation_);
		}
	}

	if (mode_ == NORMAL) {
		//制御点追加
		if (Input::GetInstance()->IsTriggerMouse(3)) {
			speedPointManager->AddNormalSpeed(transform_.translation_);
		}
	}
}


void PositionEditor::Draw(const ViewProjection& viewProjection) {
	obj3d_->Draw(transform_, viewProjection);
}

void PositionEditor::Debug() {

	ImGui::Text("3DX:%5.4f, 3DY:%5.4f, 3DZ:%5.4f", transform_.translation_.x, transform_.translation_.y, transform_.translation_.z);
	ImGui::Text("2DX:%5.4f, 2DY:%5.4f", Input::GetMousePos().x, Input::GetMousePos().y);
	ImGui::Text("0:Rail,1:Slow,2:Fast,3:Stop,4:Normal");
	ImGui::Text("Mode:%d",mode_);

	ImGui::Text("N:Rail,S:Slow,E:Fast");
	ImGui::Text("EnemyType:%d", enemyType_);
}