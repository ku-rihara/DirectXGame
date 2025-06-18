#include "EasingEditor.h"
#include <imgui.h>
#include <format>
#include<Windows.h>

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
        std::string filename = "EasingFile"; // 保存したファイル名をここに入れる
        std::string message  = std::format("{}.json saved.", filename);
        MessageBoxA(nullptr, message.c_str(), "EasingEditor", 0);
    }

    ImGui::End();
}
