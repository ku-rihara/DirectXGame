#include"FastPoint.h"

#include<imgui.h>
#include<string>

int  FastPoint::instanceNum_ = 0;

void  FastPoint::Init() {
	BaseSpeedControl::Init();
	instanceID_= instanceNum_++;
}
//更新
void  FastPoint::Update() {
	BaseSpeedControl::Update();
}
//描画
void  FastPoint::Draw(const ViewProjection&viewProjection) {
	BaseSpeedControl::Draw(viewProjection);
}

void  FastPoint::Debug() {

	if (ImGui::TreeNode(("FastPos" + std::to_string(instanceID_)).c_str())) {
		ImGui::DragFloat3(("pos" + std::to_string(instanceID_)).c_str(), &transform_.translation_.x, 0.01f);
		ImGui::TreePop();
	}
	
}

