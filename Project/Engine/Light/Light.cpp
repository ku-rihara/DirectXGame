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

    globalParameter_->CreateGroup(worldCameraGroupName_);
    RegisterWorldCameraParams();
    globalParameter_->SyncParamForGroup(worldCameraGroupName_);

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
    // シーンが切り替わっていたら、このシーン専用の構成でライティングを作り直す
    if (!pendingSceneName_.empty() && pendingSceneName_ != currentSceneName_) {
        RebuildLightsForScene(pendingSceneName_);
    }
    pendingSceneName_.clear();

    // 登録が切れてたら再登録
    if (!globalParameter_->HasRegisters(groupName_)) {
        RegisterParams();
        globalParameter_->SyncParamForGroup(groupName_);
    }
    if (!globalParameter_->HasRegisters(worldCameraGroupName_)) {
        RegisterWorldCameraParams();
        globalParameter_->SyncParamForGroup(worldCameraGroupName_);
    }

    // 鏡面反射用カメラ座標
    cameraForGPUData_->worldPosition_ = worldCameraPos_;

    // ライトの更新処理
    pointLightManager_->Update();
    spotLightManager_->Update();
    spotLightCountMax_ = int32_t(spotLightManager_->GetLightCount());

    // シェーダーが参照するライト数を同期する
    lightCountData_->pointLightCount = int(pointLightManager_->GetLightCount());
    lightCountData_->spotLightCount  = spotLightCountMax_;
}

void Light::OnSceneChanged(const std::string& sceneName) {
    // 変化したシーン名を保存
    pendingSceneName_ = sceneName;
}

std::string Light::MakeSceneFolder(const std::string& sceneName) const {
    return "Lighting/" + (sceneName.empty() ? "Common" : sceneName);
}

void Light::RebuildLightsForScene(const std::string& sceneName) {
    const std::string sceneFolder = MakeSceneFolder(sceneName);

    // 既存のPoint/SpotLightを全て破棄
    pointLightManager_->Clear();
    spotLightManager_->Clear();

    //一度クリアしてから、このシーン専用の保存データがあれば読み込む
    spotLightCountMax_  = 0;
    pointLightCountMax_ = 0;
    globalParameter_->RemoveGroup(groupName_);
    globalParameter_->CreateGroup(groupName_);
    globalParameter_->LoadFile(groupName_, sceneFolder);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    // WorldCameraもこのシーン専用の保存データがあれば読み込む
    worldCameraPos_ = Vector3();
    globalParameter_->RemoveGroup(worldCameraGroupName_);
    globalParameter_->CreateGroup(worldCameraGroupName_);
    globalParameter_->LoadFile(worldCameraGroupName_, sceneFolder);
    RegisterWorldCameraParams();
    globalParameter_->SyncParamForGroup(worldCameraGroupName_);

    // 保存されていた数だけ生成
    const int32_t spotCount  = spotLightCountMax_ > 0 ? spotLightCountMax_ : 1;
    const int32_t pointCount = pointLightCountMax_ > 0 ? pointLightCountMax_ : 1;

    for (int32_t i = 0; i < spotCount; ++i) {
        spotLightManager_->Add(dxCommon_->GetDevice().Get(), i, sceneFolder);
    }
    for (int32_t i = 0; i < pointCount; ++i) {
        pointLightManager_->Add(dxCommon_->GetDevice().Get(), sceneFolder);
    }

    lightCountData_->spotLightCount  = int(spotLightManager_->GetLightCount());
    lightCountData_->pointLightCount = int(pointLightManager_->GetLightCount());

    currentSceneName_ = sceneName;
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
    areaLightManager_->Add(dxCommon_->GetDevice().Get());

    
}

void Light::DebugImGui() {
    if (ImGui::CollapsingHeader("Light")) {

        // ライトカウント
        ImGui::SeparatorText("Count");
        ImGui::Text("Scene: %s", currentSceneName_.empty() ? "(none)" : currentSceneName_.c_str());
        if (ImGui::CollapsingHeader("LightCount")) {
            ImGui::SeparatorText("ADD / REMOVE");
            if (ImGui::Button("Add Point Light")) {
                AddPointLight();
            }
            ImGui::SameLine();
            const bool noPointLight = pointLightManager_->GetLightCount() == 0;
            ImGui::BeginDisabled(noPointLight);
            if (ImGui::Button("Remove Point Light")) {
                RemovePointLight(int(pointLightManager_->GetLightCount()) - 1);
            }
            ImGui::EndDisabled();

            if (ImGui::Button("Add Spot  Light")) {
                AddSpotLight();
            }
            ImGui::SameLine();
            const bool noSpotLight = spotLightManager_->GetLightCount() == 0;
            ImGui::BeginDisabled(noSpotLight);
            if (ImGui::Button("Remove Spot  Light")) {
                RemoveSpotLight(int(spotLightManager_->GetLightCount()) - 1);
            }
            ImGui::EndDisabled();

            // ライト数の表示
            ImGui::Text("Light Point Count: %zu", pointLightManager_->GetLightCount());
            ImGui::Text("Light Spot  Count: %zu", spotLightCountMax_);

            // このシーン専用フォルダへ保存・読込
            const std::string sceneFolder = MakeSceneFolder(currentSceneName_);
            globalParameter_->ParamSaveForImGui(groupName_, sceneFolder);
            globalParameter_->ParamLoadForImGui(groupName_, sceneFolder);
        }

        ImGui::SeparatorText("Parameter");

        ImGui::DragFloat3("WorldCamera", (float*)&worldCameraPos_, 0.01f);
        {
            // WorldCameraもこのシーン専用フォルダへ保存・読込
            const std::string sceneFolder = MakeSceneFolder(currentSceneName_);
            globalParameter_->ParamSaveForImGui(worldCameraGroupName_, sceneFolder);
            ImGui::SameLine();
            globalParameter_->ParamLoadForImGui(worldCameraGroupName_, sceneFolder);
        }
        directionalLight_->DebugImGui();
        pointLightManager_->DebugImGui();
        spotLightManager_->AdjustParams();
        areaLightManager_->DebugImGui();
        ambientLight_->DebugImGui();
    }
}

void Light::RegisterParams() {
    globalParameter_->Regist(groupName_, "spotLightCountMax", &spotLightCountMax_);
    globalParameter_->Regist(groupName_, "pointLightCountMax", &pointLightCountMax_);
}

void Light::RegisterWorldCameraParams() {
    globalParameter_->Regist(worldCameraGroupName_, "WorldCameraPos", &worldCameraPos_);
}

void Light::SetLightCommands(ID3D12GraphicsCommandList* commandList) {
    // ライトのパラメータをGPUに反映
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
    worldCameraPos_ = pos;
}

void Light::SetDitherOcclusion(const DitherOcclusionData& data) {
    *ditherOcclusionData_ = data;
}

void Light::AddSpotLight() {
    // スポットライトの追加
    const std::string sceneFolder = MakeSceneFolder(currentSceneName_);
    spotLightManager_->Add(dxCommon_->GetDevice().Get(), int32_t(spotLightManager_->GetLightCount()), sceneFolder); 
    lightCountData_->spotLightCount = int(spotLightManager_->GetLightCount());
    spotLightCountMax_              = lightCountData_->spotLightCount;
}
void Light::AddPointLight() {
    // ポイントライトの追加
    const std::string sceneFolder = MakeSceneFolder(currentSceneName_);
    pointLightManager_->Add(dxCommon_->GetDevice().Get(), sceneFolder);
    lightCountData_->pointLightCount = int(pointLightManager_->GetLightCount());
    pointLightCountMax_              = lightCountData_->pointLightCount;
}

void Light::RemoveSpotLight(int num) {
    // スポットライトの削除
    spotLightManager_->Remove(num);
    lightCountData_->spotLightCount = int(spotLightManager_->GetLightCount());
    spotLightCountMax_              = lightCountData_->spotLightCount;
}
void Light::RemovePointLight(int num) {
    // ポイントライトの削除
    pointLightManager_->Remove(num);
    lightCountData_->pointLightCount = int(pointLightManager_->GetLightCount());
    pointLightCountMax_              = lightCountData_->pointLightCount;
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
