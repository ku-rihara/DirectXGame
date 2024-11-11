#include"NormalPoint.h"

#include<imgui.h>
#include<string>

int  NormalPoint::instanceNum_ = 0;
void  NormalPoint::Init() {
	BaseSpeedControl::Init();
	instanceID_= instanceNum_++;
}
//更新
void  NormalPoint::Update() {
	BaseSpeedControl::Update();
}
//描画
void  NormalPoint::Draw(const ViewProjection&viewProjection) {
	BaseSpeedControl::Draw(viewProjection);
}

void  NormalPoint::Debug() {


	if (ImGui::TreeNode(("NormalPos" + std::to_string(instanceID_)).c_str())) {
		ImGui::DragFloat3(("pos" + std::to_string(instanceID_)).c_str(), &transform_.translation_.x, 0.01f);
		ImGui::TreePop();
	}
	
}

