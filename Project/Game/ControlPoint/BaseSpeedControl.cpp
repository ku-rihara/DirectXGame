#include"BaseSpeedControl.h"


#include<imgui.h>
#include<string>

BaseSpeedControl::BaseSpeedControl() {

}
BaseSpeedControl::~BaseSpeedControl() {

}

void  BaseSpeedControl::Init() {
	objct3D_.reset(Object3d::CreateModel("plane", ".obj"));
}

//更新
void  BaseSpeedControl::Update() {
	transform_.UpdateMatrix();
	Debug();
}

//描画
void  BaseSpeedControl::Draw(const ViewProjection&viewProjection) {
	objct3D_->Draw(transform_, viewProjection);
}

//void ControlPoint::Debug() {
//
//	/*ImGui::Begin("Positions");
//	if (ImGui::TreeNode(("SpeedPos" + std::to_string(instanceID_)).c_str())) {
//		ImGui::DragFloat3(("pos" + std::to_string(instanceID_)).c_str(), &transform_.translation_.x, 0.01f);
//		ImGui::TreePop();
//	}
//	ImGui::End();*/
//}


