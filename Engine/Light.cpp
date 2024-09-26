#include"Light.h"
#include<assert.h>
#include <imgui.h>
#include"DirectXCommon.h"

Light* Light::GetInstance() {
	static Light instance;
	return &instance;
}

void Light::Init() {
	//平行光源--------------------------------------------------------------------------------------------------
	directionalLightResource_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(DirectionalLight));
	//データ書き込む
	directionalLightData_ = nullptr;
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	//デフォルト値はこうする
	directionalLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightData_->direction = { 0.0f,-1.0f,0.0f };
	directionalLightData_->intensity = 1.0f;
	//鏡面反射-----------------------------------------------------------------------------------------------------------------
	cameraForGPUResource_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(CameraForGPU));
	//データを書き込む
	cameraForGPUData_ = nullptr;
	cameraForGPUResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraForGPUData_));
	//ポイントライト-----------------------------------------------------------------------------------------------------------------
	pointLightResource_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(PointLight));
	pointLightData_ = nullptr;
	pointLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData_));
	pointLightData_->intenesity = 1.0f;
	pointLightData_->position.y=2.0f;

}
#ifdef _DEBUG
void Light::DebugImGui() {
	ImGui::DragFloat3("WorldCamera", (float*)&cameraForGPUData_->worldPosition_, 0.01f);
	//DirectionalLight
	if (ImGui::TreeNode("DirectionalLight")) {
		ImGui::DragFloat3("Direction", (float*)&directionalLightData_->direction, 0.01f);
		ImGui::DragFloat("Intensity", (float*)&directionalLightData_->intensity, 0.1f);	
		directionalLightData_->direction = Normalize(directionalLightData_->direction);
			ImGui::TreePop();
	}
	if (ImGui::TreeNode("PointLight")) {
		//ポイントライト
		ImGui::ColorEdit4(" Color", (float*)&pointLightData_->color);
		ImGui::DragFloat3(" Pos", (float*)&pointLightData_->position, 0.01f);
		ImGui::DragFloat("  intenesity", (float*)&pointLightData_->intenesity, 0.01f);
		ImGui::DragFloat("  radius", (float*)&pointLightData_->radius, 0.01f);
		ImGui::DragFloat("  decay", (float*)&pointLightData_->decay, 0.01f);

		ImGui::TreePop();
	}
	
	/*ImGui::End();*/
}
#endif