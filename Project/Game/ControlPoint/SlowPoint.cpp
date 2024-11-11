#include"SlowPoint.h"

#include<imgui.h>
#include<string>

int  SlowPoint::instanceNum_ = 0;
void  SlowPoint::Init() {
	BaseSpeedControl::Init();
	instanceID_= instanceNum_++;
}
//更新
void  SlowPoint::Update() {
	BaseSpeedControl::Update();
}
//描画
void  SlowPoint::Draw(const ViewProjection&viewProjection) {
	BaseSpeedControl::Draw(viewProjection);
}

void  SlowPoint::Debug() {

	ImGui::Begin("SlowPositions");
	if (ImGui::TreeNode(("Rail" + std::to_string(instanceID_)).c_str())) {
		ImGui::DragFloat3(("pos" + std::to_string(instanceID_)).c_str(), &transform_.translation_.x, 0.01f);
		ImGui::TreePop();
	}
	ImGui::End();
}

