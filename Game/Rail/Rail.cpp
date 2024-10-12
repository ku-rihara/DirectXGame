#include"Rail.h"

#include<imgui.h>
int Rail::instanceNum_ = 0;
void Rail::Init() {
	instanceNum_++;
}
void Rail::Update() {
	objct3D_->Update();
	ImGui::Begin("Window");
	ImGui::DragFloat3("pos", &objct3D_->transform_.translation_.x, 0.01f);

	ImGui::End();
}
void Rail::Draw(const ViewProjection&viewProjection) {
	objct3D_->Draw(viewProjection);
}