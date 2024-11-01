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
}


void PositionEditor::PutControlPoint(ControlPointManager* controlPointManager) {

	
	//制御点追加
	if (Input::GetInstance()->IsTriggerMouse(3)) {
		controlPointManager->AddControlPoint(transform_.translation_);
	}

}

void PositionEditor::PutEnemy(EnemyManager* enemyManager) {
	if (Input::GetInstance()->IsTriggerMouse(4)) {
		enemyManager->AddNormalEnemy(transform_.translation_);
	}
}

void PositionEditor::Draw(const ViewProjection& viewProjection) {
	obj3d_->Draw(transform_, viewProjection);
}

void PositionEditor::Debug() {
	ImGui::Text("3DX:%5.4f, 3DY:%5.4f, 3DZ:%5.4f", transform_.translation_.x, transform_.translation_.y, transform_.translation_.z);
	ImGui::Text("2DX:%5.4f, 2DY:%5.4f", Input::GetMousePos().x, Input::GetMousePos().y);
}