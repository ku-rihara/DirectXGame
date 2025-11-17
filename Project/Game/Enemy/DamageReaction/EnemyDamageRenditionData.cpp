#include "EnemyDamageRenditionData.h"
#include <cassert>
#include <imgui.h>

void EnemyDamageRenditionData::BindParams(GlobalParameter* globalParam, const std::string& groupName) {
    groupName_ = groupName;

    globalParam->Regist(groupName, "_FileName", &objAnimationParams_.first.fileName);
    globalParam->Regist(groupName, "_StartTiming", &objAnimationParams_.first.startTiming);
}

void EnemyDamageRenditionData::AdjustParam() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader("Rendition Parameters")) {
        ImGui::PushID((groupName_ + "RenditionParams").c_str());

        ImGui::PopID();
    }

    // オブジェクトアニメーションパラメータのUI
    if (ImGui::CollapsingHeader("Object Animations")) {
        ImGui::PushID((groupName_ + "ObjAnimParams").c_str());

        SelectObjAnimationFile("Body", objAnimationParams_);
        ImGui::DragFloat("Start Timing", &objAnimationParams_.first.startTiming, 0.01f, 0.0f, 10.0f);
        ImGui::PopID();

        ImGui::PopID();
    }
#endif
}

void EnemyDamageRenditionData::SelectRenditionFile(
    const char* label,
    const std::string& directory,
    std::pair<RenditionParam, FileSelector>& param) {

    param.second.SelectFile(label, directory, param.first.fileName, "", true);
}

void EnemyDamageRenditionData::SelectObjAnimationFile(
    const char* label,
    std::pair<ObjAnimationParam, FileSelector>& param) {

    param.second.SelectFile(label, objAnimationFolderPath_, param.first.fileName, "", true);
}
