#include "Light.h"
#include "AmbientLight.h"
#include "AreaLightManager.h"
#include "DirectionalLight.h"
#include "Dx/DirectXCommon.h"
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

    // ライトカウント
    lightCountResource_ = DirectXCommon::GetInstance()->CreateBufferResource(dxCommon_->GetDevice(), sizeof(LightCountData));
    // データを書き込む
    lightCountData_ = nullptr;
    lightCountResource_->Map(0, nullptr, reinterpret_cast<void**>(&lightCountData_));

    // 全ライト初期化
    InitAllLights();
}

void Light::Update() {
    spotLightManager_->Update();
    spotLightCoutMax_ = int32_t(spotLightManager_->GetLightCount());
}

void Light::InitAllLights() {

    directionalLight_ = std::make_unique<DirectionalLight>();
    directionalLight_->Init(dxCommon_->GetDevice().Get());

    pointLightManager_ = std::make_unique<PointLightManager>();
    pointLightManager_->Init(dxCommon_->GetDevice().Get()); // 初期化を追加

    spotLightManager_ = std::make_unique<SpotLightManager>();
    spotLightManager_->Init(dxCommon_->GetDevice().Get()); // 初期化を追加

    areaLightManager_ = std::make_unique<AreaLightManager>();
    ambientLight_     = std::make_unique<AmbientLight>();
    ambientLight_->Init(dxCommon_->GetDevice().Get());

    for (int32_t i = 0; i < 5; ++i) {
        AddSpotLight();
    }

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

        /*ImGui::SeparatorText("Remove");
       if (ImGui::Button("Remove Point Light")) {
           RemovePointLight();
       }
       if (ImGui::Button("Remove Spot  Light")) {
           RemoveSpotLight();
       }*/

        // ライト数の表示
        ImGui::Text("Light Point Count: %zu", pointLightManager_->GetLightCount());
        ImGui::Text("Light Spot  Count: %zu", spotLightCoutMax_);

        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);
    }

    ImGui::SeparatorText("Paramater");

    ImGui::DragFloat3("WorldCamera", (float*)&cameraForGPUData_->worldPosition_, 0.01f);
    directionalLight_->DebugImGui();
    pointLightManager_->DebugImGui();
    spotLightManager_->AdJustParams();
    areaLightManager_->DebugImGui();
    ambientLight_->DebugImGui();

    ImGui::End();
}

void Light::RegisterParams() {
    globalParameter_->Regist(groupName_,"spotLightCoutMax", &spotLightCoutMax_);
}

void Light::SetLightCommands(ID3D12GraphicsCommandList* commandList) {
    directionalLight_->SetLightCommand(commandList);
    commandList->SetGraphicsRootConstantBufferView(5, cameraForGPUResource_->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(10, lightCountResource_->GetGPUVirtualAddress());
    pointLightManager_->SetLightCommand(commandList);
    spotLightManager_->SetLightCommand(commandList);
    areaLightManager_->SetLightCommand(commandList);
    ambientLight_->SetLightCommand(commandList);
}

void Light::SetWorldCameraPos(const Vector3& pos) {
    cameraForGPUData_->worldPosition_ = pos;
}

void Light::AddSpotLight() {
    spotLightManager_->Add(dxCommon_->GetDevice().Get(), int32_t(spotLightManager_->GetLightCount()));

    // データを合わせる
    lightCountData_->spotLightCount = int(spotLightCoutMax_);
}
void Light::AddPointLight() {
    pointLightManager_->Add(dxCommon_->GetDevice().Get());
    lightCountData_->pointLightCount = int(pointLightManager_->GetLightCount());
}

void Light::RemoveSpotLight(const int& num) {
    pointLightManager_->Remove(num);
    lightCountData_->pointLightCount = int(pointLightManager_->GetLightCount());
}
void Light::RemovePointLight(const int& num) {
    pointLightManager_->Remove(num);
    lightCountData_->pointLightCount = int(pointLightManager_->GetLightCount());
}