#include <format>
#include "PlayerAttackRenditionData.h"
#include <algorithm>
#include <imgui.h>

void PlayerAttackRenditionData::RegisterParams(KetaEngine::GlobalParameter* globalParam, const std::string& groupName, const std::string& prefix) {
    groupName_ = groupName;

    // 通常演出・ヒット時演出パラメータリストの登録
    for (const auto& info : kRenditionTypeInfos) {
        size_t idx = static_cast<size_t>(info.type);
        globalParam->RegistArray(groupName, prefix + std::string(info.name) + "_List", &renditionLists_[idx]);
        globalParam->RegistArray(groupName, prefix + std::string(info.name) + "_OnHit_List", &renditionOnHitLists_[idx]);
    }

    // オブジェクトアニメーションパラメータリストの登録
    for (const auto& info : kObjAnimationTypeInfos) {
        size_t idx = static_cast<size_t>(info.type);
        globalParam->RegistArray(groupName, prefix + std::string(info.name) + "_List", &objAnimationLists_[idx]);
    }

    // 振動パラメータリストの登録
    globalParam->RegistArray(groupName, prefix + "Vibration_List", &vibrationList_);
}

void PlayerAttackRenditionData::AdjustParam() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    // 通常演出・ヒット時演出パラメータのUI(Typeごとに複数行+Add/Remove)
    auto drawRenditionSection = [this](const char* headerLabel, bool onHit) {
        if (!ImGui::CollapsingHeader(headerLabel)) {
            return;
        }
        ImGui::PushID(headerLabel);

        for (const auto& info : kRenditionTypeInfos) {
            // ヒット音は通常演出に含めない
            if (!onHit && info.type == Type::AudioHit) {
                continue;
            }

            ImGui::PushID(static_cast<int>(info.type));
            auto& list = onHit ? renditionOnHitLists_[static_cast<size_t>(info.type)] : renditionLists_[static_cast<size_t>(info.type)];

            std::string sectionLabel = onHit ? (std::string(info.label) + " (On Hit)") : std::string(info.label);
            ImGui::SeparatorText(sectionLabel.c_str());

            // オーディオの場合は専用のフォルダパスを使用
            std::string directory = (info.type == Type::AudioAttack || info.type == Type::AudioHit)
                                         ? audioFolderPath_
                                         : folderPath_ + info.dir;

            for (size_t i = 0; i < list.size(); ++i) {
                ImGui::PushID(static_cast<int>(i));
                auto& param = list[i];

                std::string selectorKey = std::format("{}_{}_{}_{}", groupName_, headerLabel, info.name, i);
                KetaEngine::FileSelector& selector = fileSelectorMap_[selectorKey];
                selector.SelectFile(info.label, directory, param.fileName, "", true);

                ImGui::DragFloat("Start Timing", &param.startTiming, 0.01f, 0.0f, 10.0f);

                // CameraActionの場合のみチェックボックスを表示
                if (info.type == Type::CameraAction) {
                    ImGui::Checkbox("Is Camera Reset", &param.isCameraReset);
                }

                // オーディオの場合のみボリューム(・ヒット時はrepeatOnDamage)を表示
                if (info.type == Type::AudioAttack || info.type == Type::AudioHit) {
                    ImGui::SliderFloat("Volume", &param.volume, 0.0f, 1.0f);
                    if (onHit) {
                        ImGui::Checkbox("ダメージごとに再生", &param.repeatOnDamage);
                    }
                }

                bool removed = false;
                if (ImGui::Button("Remove")) {
                    list.erase(list.begin() + i);
                    removed = true;
                }
                ImGui::PopID();
                if (removed) {
                    break;
                }
            }

            if (ImGui::Button("+ Add")) {
                list.push_back(RenditionParam{});
            }

            ImGui::PopID();
        }

        ImGui::PopID();
    };

    drawRenditionSection("Rendition Parameters (Normal)", false);
    drawRenditionSection("Rendition Parameters (On Hit)", true);

    // オブジェクトアニメーションパラメータのUI
    if (ImGui::CollapsingHeader("Object Animations")) {
        ImGui::PushID((groupName_ + "ObjAnimParams").c_str());

        for (const auto& info : kObjAnimationTypeInfos) {
            ImGui::PushID(static_cast<int>(info.type));
            auto& list = objAnimationLists_[static_cast<size_t>(info.type)];

            ImGui::SeparatorText(info.label);
            std::string directory = GetObjAnimationFolderPath(info.type);

            for (size_t i = 0; i < list.size(); ++i) {
                ImGui::PushID(static_cast<int>(i));
                auto& param = list[i];

                std::string selectorKey = std::format("{}_ObjAnim_{}_{}", groupName_, info.name, i);
                KetaEngine::FileSelector& selector = fileSelectorMap_[selectorKey];
                selector.SelectFile(info.label, directory, param.fileName, "", true);

                ImGui::DragFloat("Start Timing", &param.startTiming, 0.01f, 0.0f, 10.0f);

                bool removed = false;
                if (ImGui::Button("Remove")) {
                    list.erase(list.begin() + i);
                    removed = true;
                }
                ImGui::PopID();
                if (removed) {
                    break;
                }
            }

            if (ImGui::Button("+ Add")) {
                list.push_back(ObjAnimationParam{});
            }

            ImGui::PopID();
        }

        ImGui::PopID();
    }

    // 振動パラメータのUI
    if (ImGui::CollapsingHeader("Vibration Parameters")) {
        ImGui::PushID((groupName_ + "VibrationParams").c_str());

        for (size_t i = 0; i < vibrationList_.size(); ++i) {
            ImGui::PushID(static_cast<int>(i));
            auto& param = vibrationList_[i];

            ImGui::SeparatorText("Vibration");
            ImGui::DragFloat("Start Timing", &param.startTiming, 0.01f, 0.0f, 10.0f);
            ImGui::DragFloat("Duration", &param.duration, 0.01f, 0.0f, 5.0f);
            ImGui::SliderFloat("Intensity", &param.intensity, 0.0f, 1.0f);
            ImGui::Checkbox("Trigger By Hit", &param.triggerByHit);
            ImGui::Checkbox("ダメージごとに振動", &param.repeatOnDamage);

            bool removed = false;
            if (ImGui::Button("Remove")) {
                vibrationList_.erase(vibrationList_.begin() + i);
                removed = true;
            }
            ImGui::PopID();
            if (removed) {
                break;
            }
        }

        if (ImGui::Button("+ Add")) {
            vibrationList_.push_back(VibrationParam{});
        }

        ImGui::PopID();
    }
#endif
}

std::string PlayerAttackRenditionData::GetObjAnimationFolderPath(ObjAnimationType type) const {
    return objAnimationFolderPaths_[static_cast<int32_t>(type)];
}
