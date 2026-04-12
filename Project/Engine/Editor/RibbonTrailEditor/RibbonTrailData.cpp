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
    } else {
        GetParams();
    }
}

void RibbonTrailData::Update(float speedRate) {
   // 未使用
    speedRate;
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
    globalParameter_->Regist(groupName_, "UVScrollSpeed",           &uvScrollSpeed_);
    globalParameter_->Regist(groupName_, "DistortionUVScrollSpeed", &distortionUVScrollSpeed_);
    globalParameter_->Regist(groupName_, "TexturePath",             &texturePath_);

    // 追従モード
    globalParameter_->Regist(groupName_, "FollowMode", &followModeInt_);

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
    uvScrollSpeed_             = globalParameter_->GetValue<Vector2>(groupName_, "UVScrollSpeed");
    distortionUVScrollSpeed_   = globalParameter_->GetValue<Vector2>(groupName_, "DistortionUVScrollSpeed");
    texturePath_               = globalParameter_->GetValue<std::string>(groupName_, "TexturePath");
    followModeInt_ = globalParameter_->GetValue<int32_t>(groupName_, "FollowMode");
    followMode_    = static_cast<TrailFollowMode>(followModeInt_);
    useDistortion_         = globalParameter_->GetValue<bool>(groupName_, "UseDistortion");
    distortionStrength_    = globalParameter_->GetValue<float>(groupName_, "DistortionStrength");
    distortionTexturePath_ = globalParameter_->GetValue<std::string>(groupName_, "DistortionTexturePath");
}

void RibbonTrailData::InitParams() {
    startColor_            = {1.0f, 0.8f, 0.3f, 1.0f};
    endColor_              = {1.0f, 0.4f, 0.1f, 0.0f};
    startWidth_            = 0.12f;
    endWidth_              = 0.0f;
    lifetime_              = 0.25f;
    maxPoints_             = 48;
    emitInterval_          = 0.005f;
    uvScrollSpeed_             = Vector2::ZeroVector();
    distortionUVScrollSpeed_   = Vector2::ZeroVector();
    texturePath_           = "";
    followMode_    = TrailFollowMode::Follow;
    followModeInt_ = 0;
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

    ImGui::SeparatorText("追従モード");
    const char* followModeItems[] = {"Follow（追従）", "StayInPlace（その場に留まる）"};
    if (ImGui::Combo("モード", &followModeInt_, followModeItems, IM_ARRAYSIZE(followModeItems))) {
        followMode_ = static_cast<TrailFollowMode>(followModeInt_);
    }

    ImGui::SeparatorText("Life / Length");
    ImGui::DragFloat("Lifetime", &lifetime_, 0.01f, 0.01f, 10.0f);
    ImGui::DragInt("Max Points", &maxPoints_, 1, 2, 256);
    ImGui::DragFloat("Emit Interval", &emitInterval_, 0.001f, 0.001f, 1.0f);

    ImGui::SeparatorText("UV Scroll");
    ImGui::DragFloat2("Scroll Speed (U/V)", &uvScrollSpeed_.x, 0.01f, -10.0f, 10.0f);

    ImGui::SeparatorText("Texture");

    textureSelector_.SelectFilePath("##TexSelect", kTextureFolderPath, texturePath_, ".dds", true);
    if (!texturePath_.empty()) {
        ImGui::TextDisabled("Path: %s", texturePath_.c_str());
    }

    ImGui::SeparatorText("時空歪み (Distortion)");
    ImGui::Checkbox("有効化", &useDistortion_);

    if (useDistortion_) {
        ImGui::DragFloat("歪み強度", &distortionStrength_, 0.01f, 0.0f, 5.0f);
        ImGui::DragFloat2("歪みスクロール (U/V)", &distortionUVScrollSpeed_.x, 0.01f, -10.0f, 10.0f);
        distortionTextureSelector_.SelectFilePath("##DistTexSelect", kDistortionFolderPath, distortionTexturePath_, ".dds", true);
        if (!distortionTexturePath_.empty()) {
            ImGui::TextDisabled("Path: %s", distortionTexturePath_.c_str());
        }
    }

    // ImGui編集値を GlobalParameter に即時反映（Playerへのリアルタイム同期用）
    globalParameter_->SyncParamForGroup(groupName_);

    ImGui::PopID();
#endif
}
