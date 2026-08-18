#include "PointLight.h"

using namespace KetaEngine;
#include"Base/Dx/DirectXCommon.h"
#include <imgui.h>


PointLight::~PointLight() {
    // 登録済みのグループを消す
    if (globalParameter_ && !groupName_.empty()) {
        globalParameter_->ClearRegistersForGroup(groupName_);
    }
}

void PointLight::Init(ID3D12Device* device, const std::string& groupName, const std::string& sceneFolder) {
    lightResource_ = DirectXCommon::GetInstance()->CreateBufferResource(device, sizeof(PointLightData));
    lightData_ = nullptr;
    lightResource_->Map(0, nullptr, reinterpret_cast<void**>(&lightData_));
    lightData_->radius     = 1.0f;
    lightData_->decay      = 0.1f;
    lightData_->intenesity = 1.0f;
    lightData_->position.y = 2.0f;

    groupName_       = groupName;
    sceneFolder_     = sceneFolder;
    globalParameter_ = GlobalParameter::GetInstance();

    // 別シーンの値が残っていないようにクリアしてから、このシーン用の保存値があれば読み込む
    globalParameter_->RemoveGroup(groupName_);
    globalParameter_->CreateGroup(groupName_);
    globalParameter_->LoadFile(groupName_, sceneFolder_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

#if defined(_DEBUG) || defined(DEVELOPMENT)
    // デバッグ表示用ライン
    debugLine_.reset(Line3D::Create(150));
#endif
}

void PointLight::Update() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    if (debugLine_) {
        // 位置と影響半径を表す球
        debugLine_->SetSphereWireframe(lightData_->position, lightData_->radius, Vector4::kGREEN());
    }
#endif
}

void PointLight::RegisterParams() {
    globalParameter_->Regist(groupName_, "Color", &lightData_->color);
    globalParameter_->Regist(groupName_, "Position", &lightData_->position);
    globalParameter_->Regist(groupName_, "intenesity", &lightData_->intenesity);
    globalParameter_->Regist(groupName_, "radius", &lightData_->radius);
    globalParameter_->Regist(groupName_, "decay", &lightData_->decay);
}

void PointLight::DebugImGui() {
    ImGui::PushID(groupName_.c_str());
    ImGui::ColorEdit4("Color", (float*)&lightData_->color);
    ImGui::DragFloat3("Pos", (float*)&lightData_->position, 0.01f);
    ImGui::DragFloat("intensity", (float*)&lightData_->intenesity, 0.01f);
    ImGui::DragFloat("radius", (float*)&lightData_->radius, 0.01f);
    ImGui::DragFloat("decay", (float*)&lightData_->decay, 0.01f);

    // 登録が切れてたら再登録
    if (!globalParameter_->HasRegisters(groupName_)) {
        RegisterParams();
    }

    globalParameter_->ParamSaveForImGui(groupName_, sceneFolder_);
    globalParameter_->ParamLoadForImGui(groupName_, sceneFolder_);
    ImGui::PopID();
}

void PointLight::SetPosition(const Vector3& pos) {
    lightData_->position = pos;
}

