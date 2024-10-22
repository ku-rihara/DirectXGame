#include"Rail.h"

#include<imgui.h>
#include<string>
int Rail::instanceNum_ = 0;
void Rail::Init() {
	objct3D_.reset(Object3d::CreateModel("cube", ".obj"));
	instanceID_= instanceNum_++;
}
//更新
void Rail::Update() {
	objct3D_->Update();
	Debug();
}
//描画
void Rail::Draw(const ViewProjection&viewProjection) {
	objct3D_->Draw(viewProjection);
}

void Rail::Debug() {

	ImGui::Begin("Window");
	ImGui::DragFloat3(("pos" + std::to_string(instanceID_)).c_str(), &objct3D_->transform_.translation_.x, 0.01f);
	ImGui::End();
}

