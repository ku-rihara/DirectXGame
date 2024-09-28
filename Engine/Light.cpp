#include"Light.h"
#include<assert.h>
#include <imgui.h>
#include<cmath>
#include <numbers>
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
	//スポットライト-----------------------------------------------------------------------------------------------------------------
	spotLightResource_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(SpotLight));
	spotLightData_ = nullptr;
	spotLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&spotLightData_));
	//パラメータ設定
	spotLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
	spotLightData_->position = { 2.0f,1.25f,0.0f };
	spotLightData_->distance = 7.0f;
	spotLightData_->direction = Normalize({ -1.0f,-1.0f,0.0f });
	spotLightData_->intensity = 4.0f;
	spotLightData_->decay = 2.0f;
	spotLightData_->cosAngle = std::cos(std::numbers::pi_v<float> / 3.0f);


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
	if (ImGui::TreeNode("SpotLight")) {
		//ポイントライト
		ImGui::ColorEdit4(" Color", (float*)&spotLightData_->color);
		ImGui::DragFloat3(" Pos", (float*)&spotLightData_->position, 0.01f);
		ImGui::DragFloat3(" Direction", (float*)&spotLightData_->direction, 0.01f);
		spotLightData_->direction = Normalize(spotLightData_->direction);
		ImGui::DragFloat("  Distance", (float*)&spotLightData_->distance, 0.01f);
		ImGui::DragFloat("  intenesity", (float*)&spotLightData_->intensity, 0.01f);
		ImGui::DragFloat("  decay", (float*)&spotLightData_->decay, 0.01f);
		ImGui::DragFloat("  cosFalloffStart", (float*)&spotLightData_->cosFalloffStart, 0.01f);

		ImGui::TreePop();
	}
	
	/*ImGui::End();*/
}
#endif