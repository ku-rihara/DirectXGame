#include "EasingEditor.h"
#include "base/WinApp.h"
#include <format>
#include <imgui.h>

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

    ImGui::End();
}

template <typename T>
void EasingEditor::SetAutoApplyTarget(Easing<T>* easing, const std::string& presetName) {
    autoPresetName_ = presetName;

    if constexpr (std::is_same_v<T, float>) {
        fTarget_  = easing;
        v2Target_ = nullptr;
        v3Target_ = nullptr;
    } else if constexpr (std::is_same_v<T, Vector2>) {
        fTarget_  = nullptr;
        v2Target_ = easing;
        v3Target_ = nullptr;
    } else if constexpr (std::is_same_v<T, Vector3>) {
        fTarget_  = nullptr;
        v2Target_ = nullptr;
        v3Target_ = easing;
    }
}

void EasingEditor::UpdatePreview() {
    if (autoPresetName_.empty())
        return;

    if (fTarget_ && fEasingCreator_.GetAllPresets().count(autoPresetName_)) {
        fTarget_->SettingValue(fEasingCreator_.GetAllPresets().at(autoPresetName_));
    } else if (v2Target_ && vec2EasingCreator_.GetAllPresets().count(autoPresetName_)) {
        v2Target_->SettingValue(vec2EasingCreator_.GetAllPresets().at(autoPresetName_));
    } else if (v3Target_ && vec3EasingCreator_.GetAllPresets().count(autoPresetName_)) {
        v3Target_->SettingValue(vec3EasingCreator_.GetAllPresets().at(autoPresetName_));
    }
}
// 明示的インスタンス化をここで行う
template void EasingEditor::SetAutoApplyTarget<float>(Easing<float>*, const std::string&);
template void EasingEditor::SetAutoApplyTarget<Vector2>(Easing<Vector2>*, const std::string&);
template void EasingEditor::SetAutoApplyTarget<Vector3>(Easing<Vector3>*, const std::string&);