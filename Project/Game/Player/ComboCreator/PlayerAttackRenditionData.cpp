#include "PlayerAttackRenditionData.h"
#include <imgui.h>

void PlayerAttackRenditionData::BindParams(GlobalParameter* globalParam, const std::string& groupName) {
    groupName_ = groupName;

    // Camera Action
    globalParam->Bind(groupName, "CameraAction_FileName", &cameraAction_.fileName);
    globalParam->Bind(groupName, "CameraAction_StartTiming", &cameraAction_.startTiming);
    globalParam->Bind(groupName, "CameraAction_TriggerByHit", &cameraAction_.triggerByHit);

    // Hit Stop
    globalParam->Bind(groupName, "HitStop_FileName", &hitStopParam_.fileName);
    globalParam->Bind(groupName, "HitStop_StartTiming", &hitStopParam_.startTiming);
    globalParam->Bind(groupName, "HitStop_TriggerByHit", &hitStopParam_.triggerByHit);

    // Shake Action
    globalParam->Bind(groupName, "ShakeAction_FileName", &shakeAction_.fileName);
    globalParam->Bind(groupName, "ShakeAction_StartTiming", &shakeAction_.startTiming);
    globalParam->Bind(groupName, "ShakeAction_TriggerByHit", &shakeAction_.triggerByHit);

    // Shake Action
    globalParam->Bind(groupName, "PostEffectParam_FileName", &postEffectParam_.fileName);
    globalParam->Bind(groupName, "PostEffectParam_StartTiming", &postEffectParam_.startTiming);
    globalParam->Bind(groupName, "PostEffectParam_TriggerByHit", &postEffectParam_.triggerByHit);
}

void PlayerAttackRenditionData::AdjustParam() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader("Rendition Parameters")) {

        ImGui::PushID((groupName_ + "RenditionParams").c_str());

        // Camera Action
        ImGui::SeparatorText("Camera Action");
        SelectRenditionFile("Camera File", folderPath_ + "CameraAnimation/AnimationData", cameraAction_);
        ImGui::DragFloat("Camera Start Timing", &cameraAction_.startTiming, 0.01f, 0.0f, 10.0f);
        ImGui::Checkbox("Camera Trigger By Hit", &cameraAction_.triggerByHit);

        // Hit Stop
        ImGui::SeparatorText("Hit Stop");
        SelectRenditionFile("HitStop File", folderPath_ + "TimeScale", hitStopParam_);
        ImGui::DragFloat("HitStop Start Timing", &hitStopParam_.startTiming, 0.01f, 0.0f, 10.0f);
        ImGui::Checkbox("HitStop Trigger By Hit", &hitStopParam_.triggerByHit);

        // Shake Action
        ImGui::SeparatorText("Shake Action");
        SelectRenditionFile("Shake File", folderPath_ + "ShakeEditor", shakeAction_);
        ImGui::DragFloat("Shake Start Timing", &shakeAction_.startTiming, 0.01f, 0.0f, 10.0f);
        ImGui::Checkbox("Shake Trigger By Hit", &shakeAction_.triggerByHit);

        // PostEffect
        ImGui::SeparatorText("PostEffect");
        SelectRenditionFile("PostEffect File", folderPath_ + "PostEffect", postEffectParam_);
        ImGui::DragFloat("Shake Start Timing", &postEffectParam_.startTiming, 0.01f, 0.0f, 10.0f);
        ImGui::Checkbox("Shake Trigger By Hit", &postEffectParam_.triggerByHit);

        ImGui::PopID();
    }
#endif // _DEBUG
}

void PlayerAttackRenditionData::SelectRenditionFile(
    const char* label,
    const std::string& directory,
    RenditionParam& param) {

    param.fileSelector.SelectFile(label, directory, param.fileName, "", true);
}