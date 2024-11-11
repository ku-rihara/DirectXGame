#include"BaseSpeedControl.h"


#include<imgui.h>
#include<string>

BaseSpeedControl::BaseSpeedControl() {

}
BaseSpeedControl::~BaseSpeedControl() {

}

void  BaseSpeedControl::Init() {
	objct3D_.reset(Object3d::CreateModel("suzanne", ".obj"));
	transform_.Init();
}

//更新
void  BaseSpeedControl::Update() {
	
	ImGui::Begin("SpeedPositions");
	Debug();
	ImGui::End();

	transform_.UpdateMatrix();
}

//描画
void  BaseSpeedControl::Draw(const ViewProjection&viewProjection) {
	objct3D_->Draw(transform_, viewProjection);
}


Vector3 BaseSpeedControl::GetBaseCenterPosition() const {
	const Vector3 offset = { 0.0f, 0.0f, 0.0f };//ローカル座標のオフセット
	// ワールド座標に変換
	Vector3 worldPos = MatrixTransform(offset, transform_.matWorld_);

	return worldPos;
}
