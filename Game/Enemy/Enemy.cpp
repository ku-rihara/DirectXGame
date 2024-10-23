#include"Enemy.h"

#include<imgui.h>
#include<string>
int  Enemy::instanceNum_ = 0;
void  Enemy::Init() {
	objct3D_.reset(Object3d::CreateModel("axis", ".obj"));
	instanceID_ = instanceNum_++;
}
//更新
void  Enemy::Update() {
	objct3D_->Update();
	Debug();
}
//描画
void  Enemy::Draw(const ViewProjection& viewProjection) {
	objct3D_->Draw(viewProjection);
}

void  Enemy::Debug() {

	ImGui::Begin("Enemies");
	ImGui::DragFloat3(("pos" + std::to_string(instanceID_)).c_str(), &objct3D_->transform_.translation_.x, 0.01f);
	ImGui::End();
}

