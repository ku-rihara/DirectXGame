#include "EasingEditor.h"
#include <format>
#include <imgui.h>
#include <Windows.h>

void EasingEditor::Init() {
    floatPath_ = kDirectoryPath_ + "float";
    vec2Path_  = kDirectoryPath_ + "vector2";
    vec3Path_  = kDirectoryPath_ + "vector3";

    LoadAll();
}

void EasingEditor::LoadAll() {

    fEasingCreator_.LoadParameter(floatPath_);
    vec2EasingCreator_.LoadParameter(vec2Path_);
    vec3EasingCreator_.LoadParameter(vec3Path_);
}

void EasingEditor::SaveAll() {
    fEasingCreator_.SaveParameter(floatPath_);
    vec2EasingCreator_.SaveParameter(vec2Path_);
    vec3EasingCreator_.SaveParameter(vec3Path_);
}

void EasingEditor::Edit() {
    ImGui::Begin("Easing Editor");

    if (ImGui::BeginTabBar("EasingTabs")) {
        if (ImGui::BeginTabItem("Float")) {
            fEasingCreator_.Edit();
            currentTab_ = TabType::Float;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Vector2")) {
            vec2EasingCreator_.Edit();
            currentTab_ = TabType::Vector2;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Vector3")) {
            vec3EasingCreator_.Edit();
            currentTab_ = TabType::Vector3;
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    if (ImGui::Button("Save All")) {
        SaveAll();
        std::string filename = "EasingFile";
        std::string message  = std::format("{}.json saved.", filename);
        MessageBoxA(nullptr, message.c_str(), "EasingEditor", 0);
    }

    if (ImGui::Button("Load All")) {
        LoadAll();
    }

    if (currentTab_ == TabType::Float) {

        // 即時反映
        if (fTarget_ && !fTarget_->GetCurrentAppliedFileName().empty()) {
            if (const auto* param = fEasingCreator_.GetEditingParam(fTarget_->GetCurrentAppliedFileName())) {
                fTarget_->SettingValue(*param);
            }
        }
    } else if (currentTab_ == TabType::Vector2) {

        if (v2Target_ && !v2Target_->GetCurrentAppliedFileName().empty()) {
            if (const auto* param = vec2EasingCreator_.GetEditingParam(v2Target_->GetCurrentAppliedFileName())) {
                v2Target_->SettingValue(*param);
            }
        }
    } else if (currentTab_ == TabType::Vector3) {

        if (v3Target_ && !v3Target_->GetCurrentAppliedFileName().empty()) {
            if (const auto* param = vec3EasingCreator_.GetEditingParam(v3Target_->GetCurrentAppliedFileName())) {
                v3Target_->SettingValue(*param);
            }
        }
    }

    ImGui::End();
}
