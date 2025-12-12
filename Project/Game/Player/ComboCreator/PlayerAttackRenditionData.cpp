#include "PlayerAttackRenditionData.h"
#include <cassert>
#include <imgui.h>

void PlayerAttackRenditionData::RegisterParams(KetaEngine::GlobalParameter* globalParam, const std::string& groupName) {
    groupName_ = groupName;

    struct RegInfo {
        Type type;
        const char* name;
    };

    // 登録する項目リスト
    const RegInfo infos[] = {
        {Type::CameraAction, "CameraAction"},
        {Type::HitStop, "HitStop"},
        {Type::ShakeAction, "ShakeAction"},
        {Type::PostEffect, "PostEffectParam"},
    };

    for (const auto& info : infos) {
        auto& param = renditionParams_[static_cast<size_t>(info.type)].first;

        globalParam->Regist(groupName, std::string(info.name) + "_FileName", &param.fileName);
        globalParam->Regist(groupName, std::string(info.name) + "_StartTiming", &param.startTiming);
        globalParam->Regist(groupName, std::string(info.name) + "_TriggerByHit", &param.triggerByHit);
    }

    // オブジェクトアニメーションパラメータの登録
    struct ObjAnimRegInfo {
        ObjAnimationType type;
        const char* name;
    };

    const ObjAnimRegInfo objAnimInfos[] = {
        {ObjAnimationType::Head, "ObjAnim_Head"},
        {ObjAnimationType::RightHand, "ObjAnim_RightHand"},
        {ObjAnimationType::LeftHand, "ObjAnim_LeftHand"},
        {ObjAnimationType::MainHead, "Main_Head"},
    };

    for (const auto& info : objAnimInfos) {
        auto& param = objAnimationParams_[static_cast<size_t>(info.type)].first;

        globalParam->Regist(groupName, std::string(info.name) + "_FileName", &param.fileName);
        globalParam->Regist(groupName, std::string(info.name) + "_StartTiming", &param.startTiming);
    }
}

void PlayerAttackRenditionData::AdjustParam() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader("Rendition Parameters")) {
        ImGui::PushID((groupName_ + "RenditionParams").c_str());

        struct UIInfo {
            Type type;
            const char* label;
            const char* dir;
        };

        const UIInfo infos[] = {
            {Type::CameraAction, "Camera Action", "CameraAnimation/AnimationData"},
            {Type::HitStop, "Hit Stop", "TimeScale"},
            {Type::ShakeAction, "Shake Action", "ShakeEditor"},
            {Type::PostEffect, "PostEffect", "PostEffect"},
        };

        for (const auto& info : infos) {
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

        struct ObjAnimUIInfo {
            ObjAnimationType type;
            const char* label;
        };

        const ObjAnimUIInfo objAnimInfos[] = {
            {ObjAnimationType::Head, "Head Animation"},
            {ObjAnimationType::RightHand, "Right Hand Animation"},
            {ObjAnimationType::LeftHand, "Left Hand Animation"},
            {ObjAnimationType::MainHead, "Main Head Animation"},
        };

        for (const auto& info : objAnimInfos) {
            ImGui::PushID(static_cast<int>(info.type));
            auto& paramPair = objAnimationParams_[static_cast<size_t>(info.type)];
            auto& param     = paramPair.first;

            ImGui::SeparatorText(info.label);
            // 各タイプに応じたフォルダパスを使用
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
    switch (type) {
    case ObjAnimationType::Head:
        return objAnimationFolderPath_Head_;
    case ObjAnimationType::RightHand:
        return objAnimationFolderPath_RightHand_;
    case ObjAnimationType::LeftHand:
        return objAnimationFolderPath_LeftHand_;
    case ObjAnimationType::MainHead:
        return objAnimationFolderPath_MainHead_;
    default:
        return objAnimationFolderPath_Head_; // デフォルト
    }
}

const PlayerAttackRenditionData::RenditionParam& PlayerAttackRenditionData::GetRenditionParamFromIndex(int32_t index) const {
    assert(index >= 0 && index < static_cast<int32_t>(Type::Count) && "Invalid Rendition Type Index");
    return GetRenditionParamFromType(static_cast<Type>(index));
}