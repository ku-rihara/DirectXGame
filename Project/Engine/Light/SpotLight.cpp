#include "SpotLight.h"

using namespace KetaEngine;
#include "Base/Dx/DirectXCommon.h"
#include <imgui.h>

SpotLight::~SpotLight() {
    // 登録済みのグループを消す
    if (globalParameter_ && !groupName_.empty()) {
        globalParameter_->ClearRegistersForGroup(groupName_);
    }
}

void SpotLight::Init(ID3D12Device* device, const std::string& groupName, const std::string& sceneFolder) {
    lightResource_ = DirectXCommon::GetInstance()->CreateBufferResource(device, sizeof(SpotLightData));
    lightData_     = nullptr;
    lightResource_->Map(0, nullptr, reinterpret_cast<void**>(&lightData_));

    // デフォルト値をセット
    lightData_->color           = {1.0f, 1.0f, 1.0f, 1.0f};
    lightData_->position        = {0.0f, 3.0f, 0.0f};
    lightData_->intensity       = 1.0f;
    lightData_->direction       = {0.0f, -1.0f, 0.0f};
    lightData_->distance        = 5.0f;
    lightData_->decay           = 0.5f;
    lightData_->cosAngle        = 0.8f;
    lightData_->cosFalloffStart = 0.9f;
    tempPos_                    = lightData_->position;

    groupName_       = groupName;
    sceneFolder_     = sceneFolder;
    globalParameter_ = GlobalParameter::GetInstance();

    // globalParameterに登録
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

void SpotLight::Update() {
    // 登録が切れてたら再登録して値を同期
    if (!globalParameter_->HasRegisters(groupName_)) {
        RegisterParams();
        globalParameter_->SyncParamForGroup(groupName_);
    }

    if (!isMove_) {
        lightData_->position = tempPos_;
    }

#if defined(_DEBUG) || defined(DEVELOPMENT)
    if (debugLine_) {
        // 位置マーカー・向きと届く距離を表す線
        debugLine_->SetSphereWireframe(lightData_->position, 0.2f, Vector4::kRED());
        Vector3 dir = lightData_->direction.Normalize();
        debugLine_->SetLine(lightData_->position, lightData_->position + dir * lightData_->distance, Vector4::kRED());
    }
#endif
 }

void SpotLight::DebugImGui() {
}

void SpotLight::RegisterParams() {
    lightData_->decay = max(lightData_->decay,0.1f);
    globalParameter_->Regist(groupName_, "Color", &lightData_->color);
    globalParameter_->Regist(groupName_, "Pos", &tempPos_);
    globalParameter_->Regist(groupName_, "Direction", &lightData_->direction);
    globalParameter_->Regist(groupName_, "Distance", &lightData_->distance);
    globalParameter_->Regist(groupName_, "intenesity", &lightData_->intensity);
    globalParameter_->Regist(groupName_, "decay", &lightData_->decay);
    globalParameter_->Regist(groupName_, "cosAngle", &lightData_->cosAngle);
    globalParameter_->Regist(groupName_, "cosFalloffStart", &lightData_->cosFalloffStart);
    globalParameter_->Regist(groupName_, "isMove", &isMove_);
}

void SpotLight::AdjustParam() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    // 登録が切れてたら再登録
    if (!globalParameter_->HasRegisters(groupName_)) {
        RegisterParams();
    }

    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());
        ImGui::Checkbox("IsMove", &isMove_);
        ImGui::ColorEdit4(" Color", (float*)&lightData_->color);
        ImGui::DragFloat3(" Pos", (float*)&tempPos_, 0.01f);
        ImGui::DragFloat3(" Direction", (float*)&lightData_->direction, 0.01f);
        lightData_->direction = lightData_->direction.Normalize();
        ImGui::DragFloat("  Distance", (float*)&lightData_->distance, 0.1f, 0.0f, 1000.0f);
        ImGui::DragFloat("  intenesity", (float*)&lightData_->intensity, 0.01f, 0.0f, 100.0f);
        ImGui::DragFloat("  decay", (float*)&lightData_->decay, 0.01f, 0.0f, 10.0f);
        ImGui::DragFloat("  cosAngle", (float*)&lightData_->cosAngle, 0.001f, 0.0f, 1.0f);
        ImGui::DragFloat("  cosFalloffStart", (float*)&lightData_->cosFalloffStart, 0.001f, 0.0f, 1.0f);

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_, sceneFolder_);
        globalParameter_->ParamLoadForImGui(groupName_, sceneFolder_);

        ImGui::PopID();
    }
#endif // _DEBUG
}

void SpotLight::SetPosition(const Vector3& pos) {
    lightData_->position = pos;
}
