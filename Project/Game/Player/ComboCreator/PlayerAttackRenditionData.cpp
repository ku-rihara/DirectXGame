#include "PlayerAttackRenditionData.h"
#include <imgui.h>
#include <cassert>

void PlayerAttackRenditionData::BindParams(GlobalParameter* globalParam, const std::string& groupName) {
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
            auto& paramPair = renditionParams_[static_cast<size_t>(info.type)];
            auto& param     = paramPair.first;

            ImGui::SeparatorText(info.label);
            SelectRenditionFile(info.label, folderPath_ + info.dir, paramPair);
            ImGui::DragFloat("Start Timing", &param.startTiming, 0.01f, 0.0f, 10.0f);
            ImGui::Checkbox("Trigger By Hit", &param.triggerByHit);
        }

        ImGui::PopID();
    }
#endif
}

void PlayerAttackRenditionData::SelectRenditionFile(
    const char* label,
    const std::string& directory,
    std::pair<RenditionParam, FileSelector>& param) {

    param.second.SelectFile(label, directory, param.first.fileName, "", true);
}

const PlayerAttackRenditionData::RenditionParam& PlayerAttackRenditionData::GetRenditionParamFromIndex(const int32_t& index) const {
    assert(index >= 0 && index < static_cast<int32_t>(Type::Count) && "Invalid Rendition Type Index");
    return GetRenditionParamFromType(static_cast<Type>(index));
}

