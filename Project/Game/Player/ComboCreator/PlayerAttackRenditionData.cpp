#include "PlayerAttackRenditionData.h"
#include <cassert>
#include <imgui.h>

void PlayerAttackRenditionData::RegisterParams(KetaEngine::GlobalParameter* globalParam, const std::string& groupName) {
    groupName_ = groupName;

    // 演出パラメータの登録
    for (const auto& info : kRenditionTypeInfos) {
        auto& param = renditionParams_[static_cast<size_t>(info.type)].first;

        globalParam->Regist(groupName, std::string(info.name) + "_FileName", &param.fileName);
        globalParam->Regist(groupName, std::string(info.name) + "_StartTiming", &param.startTiming);
        globalParam->Regist(groupName, std::string(info.name) + "_TriggerByHit", &param.triggerByHit);
    }

    // オブジェクトアニメーションパラメータの登録
    for (const auto& info : kObjAnimationTypeInfos) {
        auto& param = objAnimationParams_[static_cast<size_t>(info.type)].first;

        globalParam->Regist(groupName, std::string(info.name) + "_FileName", &param.fileName);
        globalParam->Regist(groupName, std::string(info.name) + "_StartTiming", &param.startTiming);
    }
}

void PlayerAttackRenditionData::AdjustParam() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader("Rendition Parameters")) {
        ImGui::PushID((groupName_ + "RenditionParams").c_str());

        // 演出パラメータのUI
        for (const auto& info : kRenditionTypeInfos) {
            ImGui::PushID(static_cast<int>(info.type));
            auto& paramPair = renditionParams_[static_cast<size_t>(info.type)];
            auto& param     = paramPair.first;

            ImGui::SeparatorText(info.label);
            SelectRenditionFile(info.label, folderPath_ + info.dir, paramPair);
            ImGui::DragFloat("Start Timing", &param.startTiming, 0.01f, 0.0f, 10.0f);
            ImGui::Checkbox("Trigger By Hit", &param.triggerByHit);
            ImGui::PopID();
        }

        ImGui::PopID();
    }

    // オブジェクトアニメーションパラメータのUI
    if (ImGui::CollapsingHeader("Object Animations")) {
        ImGui::PushID((groupName_ + "ObjAnimParams").c_str());

        for (const auto& info : kObjAnimationTypeInfos) {
            ImGui::PushID(static_cast<int>(info.type));
            auto& paramPair = objAnimationParams_[static_cast<size_t>(info.type)];
            auto& param     = paramPair.first;

            ImGui::SeparatorText(info.label);
            SelectObjAnimationFile(info.label, GetObjAnimationFolderPath(info.type), paramPair);
            ImGui::DragFloat("Start Timing", &param.startTiming, 0.01f, 0.0f, 10.0f);
            ImGui::PopID();
        }

        ImGui::PopID();
    }
#endif
}

void PlayerAttackRenditionData::SelectRenditionFile(
    const char* label,
    const std::string& directory,
    std::pair<RenditionParam, KetaEngine::FileSelector>& param) {

    param.second.SelectFile(label, directory, param.first.fileName, "", true);
}

void PlayerAttackRenditionData::SelectObjAnimationFile(
    const char* label,
    const std::string& directory,
    std::pair<ObjAnimationParam, KetaEngine::FileSelector>& param) {

    param.second.SelectFile(label, directory, param.first.fileName, "", true);
}

std::string PlayerAttackRenditionData::GetObjAnimationFolderPath(ObjAnimationType type) const {
    return objAnimationFolderPaths_[static_cast<int32_t>(type)];
}

const PlayerAttackRenditionData::RenditionParam& PlayerAttackRenditionData::GetRenditionParamFromIndex(int32_t index) const {
    assert(index >= 0 && index < static_cast<int32_t>(Type::Count) && "Invalid Rendition Type Index");
    return GetRenditionParamFromType(static_cast<Type>(index));
}