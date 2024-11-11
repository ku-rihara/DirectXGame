#include"StopPoint.h"

#include<imgui.h>
#include<string>

int  StopPoint::instanceNum_ = 0;
void  StopPoint::Init() {
	BaseSpeedControl::Init();
	instanceID_= instanceNum_++;
}
//更新
void  StopPoint::Update() {
	BaseSpeedControl::Update();
}
//描画
void  StopPoint::Draw(const ViewProjection&viewProjection) {
	BaseSpeedControl::Draw(viewProjection);
}

void  StopPoint::Debug() {


	if (ImGui::TreeNode(("StopPos" + std::to_string(instanceID_)).c_str())) {
		ImGui::DragFloat3(("pos" + std::to_string(instanceID_)).c_str(), &transform_.translation_.x, 0.01f);
		ImGui::TreePop();
	}
	
}

