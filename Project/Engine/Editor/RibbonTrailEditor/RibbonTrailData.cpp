#include "RibbonTrailData.h"

using namespace KetaEngine;
#include "Editor/ParameterEditor/GlobalParameter.h"
#include <filesystem>
#include <imgui.h>

void RibbonTrailData::Init(const std::string& name, const std::string& categoryName) {
    BaseEffectData::Init(name, categoryName);

    groupName_ = name;

    folderPath_ = baseFolderPath_ + categoryName_ + "/Dates";

    if (!globalParameter_->HasRegisters(groupName_)) {
        globalParameter_->CreateGroup(groupName_);
        RegisterParams();
        globalParameter_->SyncParamForGroup(groupName_);
        InitParams();
    } else {
        GetParams();
    }

    // texturePath_ が確定した後に textureStem_ を同期
    if (texturePath_.empty()) {
        textureStem_ = "None";
    } else {
        size_t sl      = texturePath_.rfind('/');
        std::string fn = (sl != std::string::npos) ? texturePath_.substr(sl + 1) : texturePath_;
        size_t dt      = fn.rfind('.');
        textureStem_   = (dt != std::string::npos) ? fn.substr(0, dt) : fn;
    }

    // distortionTexturePath_ の textureStem_ も同期
    if (distortionTexturePath_.empty()) {
        distortionTextureStem_ = "None";
    } else {
        size_t sl      = distortionTexturePath_.rfind('/');
        std::string fn = (sl != std::string::npos) ? distortionTexturePath_.substr(sl + 1) : distortionTexturePath_;
        size_t dt      = fn.rfind('.');
        distortionTextureStem_ = (dt != std::string::npos) ? fn.substr(0, dt) : fn;
    }
}

void RibbonTrailData::Update(float /*speedRate*/) {
    // パラメータのみのデータなので更新処理なし
}

void RibbonTrailData::Play() {
    playState_ = PlayState::PLAYING;
}

void RibbonTrailData::Reset() {
    playState_ = PlayState::STOPPED;
}

void RibbonTrailData::RegisterParams() {
    globalParameter_->Regist(groupName_, "StartColor", &startColor_);
    globalParameter_->Regist(groupName_, "EndColor", &endColor_);
    globalParameter_->Regist(groupName_, "StartWidth", &startWidth_);
    globalParameter_->Regist(groupName_, "EndWidth", &endWidth_);
    globalParameter_->Regist(groupName_, "Lifetime", &lifetime_);
    globalParameter_->Regist(groupName_, "MaxPoints", &maxPoints_);
    globalParameter_->Regist(groupName_, "EmitInterval", &emitInterval_);
    globalParameter_->Regist(groupName_, "TexturePath", &texturePath_);

    // 時空歪み
    globalParameter_->Regist(groupName_, "UseDistortion",         &useDistortion_);
    globalParameter_->Regist(groupName_, "DistortionStrength",    &distortionStrength_);
    globalParameter_->Regist(groupName_, "DistortionTexturePath", &distortionTexturePath_);
}

void RibbonTrailData::GetParams() {
    if (!globalParameter_->HasGroup(groupName_)) {
        return;
    }
    startColor_   = globalParameter_->GetValue<Vector4>(groupName_, "StartColor");
    endColor_     = globalParameter_->GetValue<Vector4>(groupName_, "EndColor");
    startWidth_   = globalParameter_->GetValue<float>(groupName_, "StartWidth");
    endWidth_     = globalParameter_->GetValue<float>(groupName_, "EndWidth");
    lifetime_     = globalParameter_->GetValue<float>(groupName_, "Lifetime");
    maxPoints_    = globalParameter_->GetValue<int32_t>(groupName_, "MaxPoints");
    emitInterval_ = globalParameter_->GetValue<float>(groupName_, "EmitInterval");
    texturePath_            = globalParameter_->GetValue<std::string>(groupName_, "TexturePath");
    useDistortion_          = globalParameter_->GetValue<bool>(groupName_, "UseDistortion");
    distortionStrength_     = globalParameter_->GetValue<float>(groupName_, "DistortionStrength");
    distortionTexturePath_  = globalParameter_->GetValue<std::string>(groupName_, "DistortionTexturePath");
}

void RibbonTrailData::InitParams() {

    startColor_   = {1.0f, 0.8f, 0.3f, 1.0f};
    endColor_     = {1.0f, 0.4f, 0.1f, 0.0f};
    startWidth_   = 0.12f;
    endWidth_     = 0.0f;
    lifetime_     = 0.25f;
    maxPoints_    = 48;
    emitInterval_          = 0.005f;
    texturePath_           = "";
    useDistortion_         = false;
    distortionStrength_    = 0.1f;
    distortionTexturePath_ = "";
}

void RibbonTrailData::AdjustParam() {
#ifdef _DEBUG
    ImGui::SeparatorText(("RibbonTrail: " + groupName_).c_str());
    ImGui::PushID(groupName_.c_str());

    DrawPlayButton();
    ImGui::Separator();

    ImGui::SeparatorText("Color");
    ImGui::ColorEdit4("Start Color (Head)", &startColor_.x);
    ImGui::ColorEdit4("End Color   (Tail)", &endColor_.x);

    ImGui::SeparatorText("Width");
    ImGui::DragFloat("Start Width (Head)", &startWidth_, 0.001f, 0.001f, 10.0f);
    ImGui::DragFloat("End Width   (Tail)", &endWidth_, 0.001f, 0.0f, 10.0f);

    ImGui::SeparatorText("Life / Length");
    ImGui::DragFloat("Lifetime", &lifetime_, 0.01f, 0.01f, 10.0f);
    ImGui::DragInt("Max Points", &maxPoints_, 1, 2, 256);
    ImGui::DragFloat("Emit Interval", &emitInterval_, 0.001f, 0.001f, 1.0f);

    ImGui::SeparatorText("Texture");

    std::string prevStem = textureStem_;
    textureSelector_.SelectFile("##TexSelect", kTextureFolderPath, textureStem_, "", true);

    // 選択が変わったら texturePath_ を再構築
    if (textureStem_ != prevStem) {
        if (textureStem_.empty() || textureStem_ == "None") {
            texturePath_ = "";
        } else {

            std::string candidate = std::string(kTextureFolderPath) + "/" + textureStem_ + ".dds";
            if (std::filesystem::exists(candidate)) {
                texturePath_ = candidate;
            }
        }
    }

    if (!texturePath_.empty()) {
        ImGui::TextDisabled("Path: %s", texturePath_.c_str());
    }

    ImGui::SeparatorText("時空歪み (Distortion)");
    ImGui::Checkbox("有効化", &useDistortion_);

    if (useDistortion_) {
        ImGui::DragFloat("歪み強度", &distortionStrength_, 0.01f, 0.0f, 5.0f);

        std::string prevDistStem = distortionTextureStem_;
        distortionTextureSelector_.SelectFile("##DistTexSelect", kTextureFolderPath, distortionTextureStem_, "", true);

        if (distortionTextureStem_ != prevDistStem) {
            if (distortionTextureStem_.empty() || distortionTextureStem_ == "None") {
                distortionTexturePath_ = "";
            } else {
                std::string candidate = std::string(kTextureFolderPath) + "/" + distortionTextureStem_ + ".dds";
                if (std::filesystem::exists(candidate)) {
                    distortionTexturePath_ = candidate;
                }
            }
        }

        if (!distortionTexturePath_.empty()) {
            ImGui::TextDisabled("Path: %s", distortionTexturePath_.c_str());
        }
    }

    ImGui::PopID();
#endif
}
