#include "VisualParameters.h"
#include "Function/GetFile.h"
#include <imgui.h>

using namespace KetaEngine;

void VisualParameters::RegisterParams(GlobalParameter* globalParam, const std::string& groupName) {
    // UV
    globalParam->Regist(groupName, "UV Pos", &uvParam_.pos);
    globalParam->Regist(groupName, "UV Rotate", &uvParam_.rotate);
    globalParam->Regist(groupName, "UV NumOfFrame", &uvParam_.numOfFrame);
    globalParam->Regist(groupName, "UV ScrollSpeed", &uvParam_.frameScrollSpeed);
    globalParam->Regist(groupName, "UV IsLoop", &uvParam_.isLoop);
    globalParam->Regist(groupName, "UV isScrollEachPixel", &uvParam_.isScrollEachPixel);
    globalParam->Regist(groupName, "UV isScroll", &uvParam_.isScroll);
    globalParam->Regist(groupName, "UV isFlipX", &uvParam_.isFlipX);
    globalParam->Regist(groupName, "UV isFlipY", &uvParam_.isFlipY);

    // Color
    globalParam->Regist(groupName, "BaseColor", &baseColor_);
    globalParam->Regist(groupName, "Color Max", &colorDist_.max);
    globalParam->Regist(groupName, "Color Min", &colorDist_.min);

    // Texture
    globalParam->Regist(groupName, "selectedTexturePath", &selectedTexturePath_);
}

void VisualParameters::AdaptParameters(GlobalParameter* globalParam, const std::string& groupName) {
    // UV
    uvParam_.pos               = globalParam->GetValue<Vector2>(groupName, "UV Pos");
    uvParam_.rotate            = globalParam->GetValue<Vector3>(groupName, "UV Rotate");
    uvParam_.numOfFrame        = globalParam->GetValue<int32_t>(groupName, "UV NumOfFrame");
    uvParam_.frameScrollSpeed  = globalParam->GetValue<float>(groupName, "UV ScrollSpeed");
    uvParam_.isLoop            = globalParam->GetValue<bool>(groupName, "UV IsLoop");
    uvParam_.isScrollEachPixel = globalParam->GetValue<bool>(groupName, "UV isScrollEachPixel");
    uvParam_.isScroll          = globalParam->GetValue<bool>(groupName, "UV isScroll");
    uvParam_.isFlipX           = globalParam->GetValue<bool>(groupName, "UV isFlipX");
    uvParam_.isFlipY           = globalParam->GetValue<bool>(groupName, "UV isFlipY");

    // Color
    baseColor_     = globalParam->GetValue<Vector4>(groupName, "BaseColor");
    colorDist_.max = globalParam->GetValue<Vector4>(groupName, "Color Max");
    colorDist_.min = globalParam->GetValue<Vector4>(groupName, "Color Min");

    // Texture
    selectedTexturePath_ = globalParam->GetValue<std::string>(groupName, "selectedTexturePath");
}

void VisualParameters::AdjustParam() {
#ifdef _DEBUG
    // Color
    if (ImGui::CollapsingHeader("Color")) {
        ImGui::SeparatorText("Base Color:");
        ImGui::ColorEdit4("Base", &baseColor_.x);

        ImGui::SeparatorText("Color Range:");
        ImGui::ColorEdit4("Max", &colorDist_.max.x);
        ImGui::ColorEdit4("Min", &colorDist_.min.x);
    }

    // UV
    if (ImGui::CollapsingHeader("UV Parameters")) {
        ImGui::SeparatorText("UV Position:");
        ImGui::DragFloat2("UV_Pos", &uvParam_.pos.x, 0.01f);

        ImGui::SeparatorText("UV Rotation:");
        ImGui::DragFloat3("UV_Rotate", &uvParam_.rotate.x, 0.1f);

        ImGui::SeparatorText("UV Animation:");
        ImGui::InputInt("Num of Frames", &uvParam_.numOfFrame);
        ImGui::DragFloat("Scroll Speed", &uvParam_.frameScrollSpeed, 0.01f);
        ImGui::Checkbox("Is Loop", &uvParam_.isLoop);
        ImGui::Checkbox("Is ScrollEachPixel", &uvParam_.isScrollEachPixel);
        ImGui::Checkbox("Is Scroll", &uvParam_.isScroll);
        ImGui::Checkbox("Is IsFlipX", &uvParam_.isFlipX);
        ImGui::Checkbox("Is IsFlipY", &uvParam_.isFlipY);
    }

    ImGuiTextureSelection();
#endif
}

void VisualParameters::ImGuiTextureSelection() {
    static int selectedIndex               = 0;
    static std::string lastSelectedTexture = "";

    std::vector<std::string> filenames = GetFileNamesForDirectory(textureFilePath_);

    if (ImGui::CollapsingHeader("SelectTexture")) {
        if (!filenames.empty()) {
            std::vector<const char*> names;
            for (const auto& file : filenames) {
                names.push_back(file.c_str());
            }

            if (ImGui::ListBox("Textures", &selectedIndex, names.data(), static_cast<int>(names.size()))) {
                std::string newTextureName = filenames[selectedIndex];
                if (newTextureName != lastSelectedTexture) {
                    ApplyTexture(newTextureName);
                    lastSelectedTexture = newTextureName;

                    if (onTextureChanged_) {
                        onTextureChanged_();
                    }
                }
            }

            if (!selectedTexturePath_.empty()) {
                ImGui::Text("Current: %s", selectedTexturePath_.c_str());
            }
        } else {
            ImGui::Text("No texture files found.");
        }
    }
}

void VisualParameters::ApplyTexture(const std::string& textureName) {
    selectedTexturePath_ = textureFilePath_ + "/" + textureName + ".png";
}

void VisualParameters::InitAdaptTexture() {
    if (selectedTexturePath_.empty()) {
        selectedTexturePath_ = textureFilePath_ + "/" + "uvChecker" + ".png";
    }
}