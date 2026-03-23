#include "PlayerAttackRenditionData.h"
#include <algorithm>
#include <cassert>
#include <imgui.h>

void PlayerAttackRenditionData::RegisterParams(KetaEngine::GlobalParameter* globalParam, const std::string& groupName) {
    groupName_ = groupName;

    // 通常演出パラメータの登録
    for (const auto& info : kRenditionTypeInfos) {
        auto& param = renditionParams_[static_cast<size_t>(info.type)].first;

        globalParam->Regist(groupName, std::string(info.name) + "_FileName", &param.fileName);
        globalParam->Regist(groupName, std::string(info.name) + "_StartTiming", &param.startTiming);

        // CameraActionの場合のみisCameraResetを登録
        if (info.type == Type::CameraAction) {
            globalParam->Regist(groupName, std::string(info.name) + "_IsCameraReset", &param.isCameraReset);
        }

        // オーディオの場合のみvolumeを登録
        if (info.type == Type::AudioAttack || info.type == Type::AudioHit) {
            globalParam->Regist(groupName, std::string(info.name) + "_Volume", &param.volume);
        }
    }

    // ヒット時演出パラメータの登録
    for (const auto& info : kRenditionTypeInfos) {
        auto& param = renditionParamsOnHit_[static_cast<size_t>(info.type)].first;

        globalParam->Regist(groupName, std::string(info.name) + "_OnHit_FileName", &param.fileName);
        globalParam->Regist(groupName, std::string(info.name) + "_OnHit_StartTiming", &param.startTiming);

        // CameraActionの場合のみisCameraResetを登録
        if (info.type == Type::CameraAction) {
            globalParam->Regist(groupName, std::string(info.name) + "_OnHit_IsCameraReset", &param.isCameraReset);
        }

        // オーディオの場合のみvolumeとrepeatOnDamageを登録
        if (info.type == Type::AudioAttack || info.type == Type::AudioHit) {
            globalParam->Regist(groupName, std::string(info.name) + "_OnHit_Volume", &param.volume);
            globalParam->Regist(groupName, std::string(info.name) + "_OnHit_RepeatOnDamage", &param.repeatOnDamage);
        }
    }

    // オブジェクトアニメーションパラメータの登録
    for (const auto& info : kObjAnimationTypeInfos) {
        auto& param = objAnimationParams_[static_cast<size_t>(info.type)].first;

        globalParam->Regist(groupName, std::string(info.name) + "_FileName", &param.fileName);
        globalParam->Regist(groupName, std::string(info.name) + "_StartTiming", &param.startTiming);

        // 右手・左手のみトレイルファイル名を登録
        if (info.type == ObjAnimationType::RightHand || info.type == ObjAnimationType::LeftHand) {
            globalParam->Regist(groupName, std::string(info.name) + "_TrailFileName", &param.trailFileName);
        }
    }

    // 振動パラメータの登録
    globalParam->Regist(groupName, "Vibration_StartTiming",   &vibrationParam_.startTiming);
    globalParam->Regist(groupName, "Vibration_Duration",      &vibrationParam_.duration);
    globalParam->Regist(groupName, "Vibration_Intensity",     &vibrationParam_.intensity);
    globalParam->Regist(groupName, "Vibration_TriggerByHit",  &vibrationParam_.triggerByHit);
    globalParam->Regist(groupName, "Vibration_RepeatOnDamage",&vibrationParam_.repeatOnDamage);

    // ポストエフェクトスロット（複数保存用）
    globalParam->Regist(groupName, "PostEffect_Count", &postEffectCount_);
    globalParam->Regist(groupName, "PostEffect_OnHit_Count", &postEffectOnHitCount_);
    for (int32_t i = 0; i < kMaxPostEffects; ++i) {
        std::string suffix = "_" + std::to_string(i);
        globalParam->Regist(groupName, "PostEffect_Slot" + suffix + "_FileName",     &postEffectSlots_[i].fileName);
        globalParam->Regist(groupName, "PostEffect_Slot" + suffix + "_StartTiming",  &postEffectSlots_[i].startTiming);
        globalParam->Regist(groupName, "PostEffect_OnHit_Slot" + suffix + "_FileName",    &postEffectOnHitSlots_[i].fileName);
        globalParam->Regist(groupName, "PostEffect_OnHit_Slot" + suffix + "_StartTiming", &postEffectOnHitSlots_[i].startTiming);
    }
}

void PlayerAttackRenditionData::SyncListToSlots() {
    postEffectCount_ = static_cast<int32_t>((std::min)(postEffectList_.size(), static_cast<size_t>(kMaxPostEffects)));
    for (int32_t i = 0; i < postEffectCount_; ++i) {
        postEffectSlots_[i] = postEffectList_[i];
    }
    // 残りスロットをクリア
    for (int32_t i = postEffectCount_; i < kMaxPostEffects; ++i) {
        postEffectSlots_[i] = RenditionParam{};
    }

    postEffectOnHitCount_ = static_cast<int32_t>((std::min)(postEffectOnHitList_.size(), static_cast<size_t>(kMaxPostEffects)));
    for (int32_t i = 0; i < postEffectOnHitCount_; ++i) {
        postEffectOnHitSlots_[i] = postEffectOnHitList_[i];
    }
    for (int32_t i = postEffectOnHitCount_; i < kMaxPostEffects; ++i) {
        postEffectOnHitSlots_[i] = RenditionParam{};
    }
}

void PlayerAttackRenditionData::SyncSlotsToList() {
    postEffectList_.clear();
    int32_t count = (std::min)(postEffectCount_, kMaxPostEffects);
    for (int32_t i = 0; i < count; ++i) {
        if (!postEffectSlots_[i].fileName.empty() && postEffectSlots_[i].fileName != "None") {
            postEffectList_.push_back(postEffectSlots_[i]);
        }
    }

    postEffectOnHitList_.clear();
    int32_t onHitCount = (std::min)(postEffectOnHitCount_, kMaxPostEffects);
    for (int32_t i = 0; i < onHitCount; ++i) {
        if (!postEffectOnHitSlots_[i].fileName.empty() && postEffectOnHitSlots_[i].fileName != "None") {
            postEffectOnHitList_.push_back(postEffectOnHitSlots_[i]);
        }
    }
}

void PlayerAttackRenditionData::AdjustParam() {
#ifdef _DEBUG
    // 通常演出パラメータのUI
    if (ImGui::CollapsingHeader("Rendition Parameters (Normal)")) {
        ImGui::PushID((groupName_ + "RenditionParams").c_str());

        for (const auto& info : kRenditionTypeInfos) {
            // ヒット音は通常演出に含めない
            if (info.type == Type::AudioHit) {
                continue;
            }

            ImGui::PushID(static_cast<int>(info.type));
            auto& paramPair = renditionParams_[static_cast<size_t>(info.type)];
            auto& param     = paramPair.first;

            ImGui::SeparatorText(info.label);

            // オーディオの場合は専用のフォルダパスを使用
            std::string directory;
            if (info.type == Type::AudioAttack || info.type == Type::AudioHit) {
                directory = audioFolderPath_;
            } else {
                directory = folderPath_ + info.dir;
            }

            SelectRenditionFile(info.label, directory, paramPair);
            ImGui::DragFloat("Start Timing", &param.startTiming, 0.01f, 0.0f, 10.0f);

            // CameraActionの場合のみチェックボックスを表示
            if (info.type == Type::CameraAction) {
                ImGui::Checkbox("Is Camera Reset", &param.isCameraReset);
            }

            // オーディオの場合のみボリュームスライダーを表示
            if (info.type == Type::AudioAttack || info.type == Type::AudioHit) {
                ImGui::SliderFloat("Volume", &param.volume, 0.0f, 1.0f);
            }

            ImGui::PopID();
        }

        ImGui::PopID();
    }

    // ヒット時演出パラメータのUI
    if (ImGui::CollapsingHeader("Rendition Parameters (On Hit)")) {
        ImGui::PushID((groupName_ + "RenditionParamsOnHit").c_str());

        for (const auto& info : kRenditionTypeInfos) {
            ImGui::PushID(static_cast<int>(info.type));
            auto& paramPair = renditionParamsOnHit_[static_cast<size_t>(info.type)];
            auto& param     = paramPair.first;

            ImGui::SeparatorText((std::string(info.label) + " (On Hit)").c_str());

            // オーディオの場合は専用のフォルダパスを使用
            std::string directory;
            if (info.type == Type::AudioAttack || info.type == Type::AudioHit) {
                directory = audioFolderPath_;
            } else {
                directory = folderPath_ + info.dir;
            }

            SelectRenditionFile(info.label, directory, paramPair);
            ImGui::DragFloat("Start Timing", &param.startTiming, 0.01f, 0.0f, 10.0f);

            // CameraActionの場合のみチェックボックスを表示
            if (info.type == Type::CameraAction) {
                ImGui::Checkbox("Is Camera Reset", &param.isCameraReset);
            }

            // オーディオの場合のみボリュームスライダーとrepeatOnDamageを表示
            if (info.type == Type::AudioAttack || info.type == Type::AudioHit) {
                ImGui::SliderFloat("Volume", &param.volume, 0.0f, 1.0f);
                ImGui::Checkbox("ダメージごとに再生", &param.repeatOnDamage);
            }

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

    // 振動パラメータのUI
    if (ImGui::CollapsingHeader("Vibration Parameters")) {
        ImGui::PushID((groupName_ + "VibrationParams").c_str());

        ImGui::SeparatorText("Vibration");
        ImGui::DragFloat("Start Timing", &vibrationParam_.startTiming, 0.01f, 0.0f, 10.0f);
        ImGui::DragFloat("Duration", &vibrationParam_.duration, 0.01f, 0.0f, 5.0f);
        ImGui::SliderFloat("Intensity", &vibrationParam_.intensity, 0.0f, 1.0f);
        ImGui::Checkbox("Trigger By Hit", &vibrationParam_.triggerByHit);
        ImGui::Checkbox("ダメージごとに振動", &vibrationParam_.repeatOnDamage);

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