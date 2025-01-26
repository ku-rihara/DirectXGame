// Light.cpp
#include "Light.h"
#include "DirectionalLight.h"
#include "PointLightManager.h"
#include "SpotLightManager.h"
#include"AreaLightManager.h"
#include "base/DirectXCommon.h"
#include <imgui.h>
#include <vector>

// Light クラスの実装
Light* Light::GetInstance() {
	static Light instance;
	return &instance;
}

void Light::Init(ID3D12Device* device) {
	directionalLight_ = std::make_unique<DirectionalLight>();
	directionalLight_->Init(device);

	//虚面反射-----------------------------------------------------------------------------------------------------------------
	cameraForGPUResource_ = DirectXCommon::GetInstance()->CreateBufferResource(device, sizeof(CameraForGPU));
	//データを書き込む
	cameraForGPUData_ = nullptr;
	cameraForGPUResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraForGPUData_));

	pointLightManager_ = std::make_unique<PointLightManager>();
	spotLightManager_ = std::make_unique<SpotLightManager>();
	areaLightManager_= std::make_unique<AreaLightManager>();

	// 初期ライトを一つ追加
	pointLightManager_->Add(device);
	spotLightManager_->Add(device);
	areaLightManager_->Add(device);
}

void Light::DebugImGui() {
	ImGui::DragFloat3("WorldCamera", (float*)&cameraForGPUData_->worldPosition_, 0.01f);
	directionalLight_->DebugImGui();
	if (ImGui::TreeNode("PointLights")) { /// point
		// const auto& に変更
		const auto& pointLights = pointLightManager_->GetLights();
		for (size_t i = 0; i < pointLights.size(); ++i) {
			if (ImGui::TreeNode(("PointLight" + std::to_string(i)).c_str())) {
				//ポインタでアクセスするように変更
				pointLights[i]->DebugImGui();
				ImGui::TreePop();
			}
		}
		if (ImGui::Button("Add Point Light")) {
			pointLightManager_->Add(DirectXCommon::GetInstance()->GetDevice());
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("SpotLights")) {/// spot
		// const auto& に変更
		const auto& spotLights = spotLightManager_->GetLights();
		for (size_t i = 0; i < spotLights.size(); ++i) {
			if (ImGui::TreeNode(("SpotLight" + std::to_string(i)).c_str())) {
				//ポインタでアクセスするように変更
				spotLights[i]->DebugImGui();
				ImGui::TreePop();
			}
		}
		if (ImGui::Button("Add Spot Light")) {
			spotLightManager_->Add(DirectXCommon::GetInstance()->GetDevice());
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("AreaLight")) {   /// AreaLight
		// const auto& に変更
		const auto& areaLights = areaLightManager_->GetLights();
		for (size_t i = 0; i < areaLights.size(); ++i) {
			if (ImGui::TreeNode(("AreaLight" + std::to_string(i)).c_str())) {
				//ポインタでアクセスするように変更
				areaLights[i]->DebugImGui();
				ImGui::TreePop();
			}
		}
		if (ImGui::Button("Add Area Light")) {
			areaLightManager_->Add(DirectXCommon::GetInstance()->GetDevice());
		}
		ImGui::TreePop();
	}
}


ID3D12Resource* Light::GetDirectionalLightResource() const {
	return directionalLight_->GetLightResource();
}

ID3D12Resource* Light::GetCameraForGPUResources() const {
	return cameraForGPUResource_.Get();
}

void  Light::SetLightCommands(ID3D12GraphicsCommandList* commandList) {
	directionalLight_->SetLightCommand(commandList);
	commandList->SetGraphicsRootConstantBufferView(4, cameraForGPUResource_->GetGPUVirtualAddress());
	pointLightManager_->SetLightCommand(commandList);
	spotLightManager_->SetLightCommand(commandList);
	areaLightManager_->SetLightCommand(commandList);
}



void Light::SetWorldCameraPos(const Vector3& pos) {
	cameraForGPUData_->worldPosition_ = pos;
}

ID3D12Resource* Light::GetCameraForGPUResource() const {
	return cameraForGPUResource_.Get();
}