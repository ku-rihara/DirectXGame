#include "Light.h"

using namespace KetaEngine;
#include "AmbientLight.h"
#include "AreaLightManager.h"
#include "Pipeline/Object3D/Object3DPipeline.h"
#include "DirectionalLight.h"
#include "Base/Dx/DirectXCommon.h"
#include "PointLightManager.h"
#include "SpotLightManager.h"
#include <imgui.h>

Light* Light::GetInstance() {
    static Light instance;
    return &instance;
}

void Light::Init(DirectXCommon* dxCommon) {

    dxCommon_ = dxCommon;

    ///* グローバルパラメータ
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    // 鏡面反射
    cameraForGPUResource_ = DirectXCommon::GetInstance()->CreateBufferResource(dxCommon_->GetDevice(), sizeof(CameraForGPU));
    // データを書き込む
    cameraForGPUData_ = nullptr;
    cameraForGPUResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraForGPUData_));

    // ディザオクルージョン
    ditherOcclusionResource_ = DirectXCommon::GetInstance()->CreateBufferResource(dxCommon_->GetDevice(), sizeof(DitherOcclusionData));
    ditherOcclusionData_ = nullptr;
    ditherOcclusionResource_->Map(0, nullptr, reinterpret_cast<void**>(&ditherOcclusionData_));
    ditherOcclusionData_->enabled = 0;

    // ライトカウント
    lightCountResource_ = DirectXCommon::GetInstance()->CreateBufferResource(dxCommon_->GetDevice(), sizeof(LightCountData));
    // データを書き込む
    lightCountData_ = nullptr;
    lightCountResource_->Map(0, nullptr, reinterpret_cast<void**>(&lightCountData_));

    // 全ライト初期化
    InitAllLights();
}

void Light::Update() {
    // ライトの更新処理
    spotLightManager_->Update();
    spotLightCountMax_ = int32_t(spotLightManager_->GetLightCount());
}

void Light::InitAllLights() {

    // ディレクショナルライト初期化
    directionalLight_ = std::make_unique<DirectionalLight>();
    directionalLight_->Init(dxCommon_->GetDevice().Get());

    // ポイントライト初期化
    pointLightManager_ = std::make_unique<PointLightManager>();
    pointLightManager_->Init(dxCommon_->GetDevice().Get());

    // スポットライト初期化
    spotLightManager_ = std::make_unique<SpotLightManager>();
    spotLightManager_->Init(dxCommon_->GetDevice().Get());

    // エリアライト初期化
    areaLightManager_ = std::make_unique<AreaLightManager>();
    ambientLight_     = std::make_unique<AmbientLight>();
    ambientLight_->Init(dxCommon_->GetDevice().Get());

    // ライトカウント初期化
    for (int32_t i = 0; i < 5; ++i) {
        AddSpotLight();
    }

    // ライトカウント初期化
    AddPointLight();
    areaLightManager_->Add(dxCommon_->GetDevice().Get());
}

void Light::DebugImGui() {
    ImGui::Begin("Light");

    // ライトカウント
    ImGui::SeparatorText("Count");
    if (ImGui::CollapsingHeader("LightCount")) {
        ImGui::SeparatorText("ADD");
        if (ImGui::Button("Add Point Light")) {
            AddPointLight();
        }
        if (ImGui::Button("Add Spot  Light")) {
            AddSpotLight();
        }

        // ライト数の表示
        ImGui::Text("Light Point Count: %zu", pointLightManager_->GetLightCount());
        ImGui::Text("Light Spot  Count: %zu", spotLightCountMax_);

        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);
    }

    ImGui::SeparatorText("Parameter");

    ImGui::DragFloat3("WorldCamera", (float*)&cameraForGPUData_->worldPosition_, 0.01f);
    directionalLight_->DebugImGui();
    pointLightManager_->DebugImGui();
    spotLightManager_->AdjustParams();
    areaLightManager_->DebugImGui();
    ambientLight_->DebugImGui();

    ImGui::End();
}

void Light::RegisterParams() {
    globalParameter_->Regist(groupName_, "spotLightCountMax", &spotLightCountMax_);
}

void Light::SetLightCommands(ID3D12GraphicsCommandList* commandList) {
    directionalLight_->SetLightCommand(commandList);
    commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(Object3DRootParameter::Camera), cameraForGPUResource_->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(Object3DRootParameter::LightCountData), lightCountResource_->GetGPUVirtualAddress());
    pointLightManager_->SetLightCommand(commandList);
    spotLightManager_->SetLightCommand(commandList);
    areaLightManager_->SetLightCommand(commandList);
    ambientLight_->SetLightCommand(commandList);
    commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(Object3DRootParameter::DitherOcclusion), ditherOcclusionResource_->GetGPUVirtualAddress());
}

void Light::SetWorldCameraPos(const Vector3& pos) {
    cameraForGPUData_->worldPosition_ = pos;
}

void Light::SetDitherOcclusion(const DitherOcclusionData& data) {
    *ditherOcclusionData_ = data;
}

void Light::AddSpotLight() {
    spotLightManager_->Add(dxCommon_->GetDevice().Get(), int32_t(spotLightManager_->GetLightCount()));

    // データを合わせる
    lightCountData_->spotLightCount = int(spotLightCountMax_);
}
void Light::AddPointLight() {
    pointLightManager_->Add(dxCommon_->GetDevice().Get());
    lightCountData_->pointLightCount = int(pointLightManager_->GetLightCount());
}

void Light::RemoveSpotLight(int num) {
    spotLightManager_->Remove(num);
    lightCountData_->spotLightCount = int(spotLightManager_->GetLightCount());
}
void Light::RemovePointLight(int num) {
    pointLightManager_->Remove(num);
    lightCountData_->pointLightCount = int(pointLightManager_->GetLightCount());
}

void Light::Finalize() {
    directionalLight_.reset();
    pointLightManager_.reset();
    spotLightManager_.reset();
    areaLightManager_.reset();
    ambientLight_.reset();

    cameraForGPUResource_.Reset();
    ditherOcclusionResource_.Reset();
    lightCountResource_.Reset();
}
