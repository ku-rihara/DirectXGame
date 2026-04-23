#include "TimelineDrawParameter.h"
#include <imgui.h>

using namespace KetaEngine;

void TimelineDrawParameter::Init(const std::string& name) {
    groupName_  = name;
    folderPath_ = "TimelineDrawer/" + name + "/Dates";

    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);
}

void TimelineDrawParameter::RegisterParams() {
    globalParameter_->Regist(groupName_, "headerWidth", &headerWidth_);
    globalParameter_->Regist(groupName_, "trackHeight", &trackHeight_);
    globalParameter_->Regist(groupName_, "rulerHeight", &rulerHeight_);
}

void TimelineDrawParameter::DrawImGui() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    std::string windowName = "TimelineDrawParameter: " + groupName_;
    if (ImGui::Begin(windowName.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::DragFloat("HeaderWidth", &headerWidth_, 1.0f, 10.0f, 500.0f);
        ImGui::DragFloat("TrackHeight", &trackHeight_, 0.5f, 10.0f, 200.0f);
        ImGui::DragFloat("RulerHeight", &rulerHeight_, 0.5f, 10.0f, 100.0f);

        ImGui::Separator();

        globalParameter_->ParamSaveForImGui(groupName_, folderPath_);

        ImGui::SameLine();
        globalParameter_->ParamLoadForImGui(groupName_, folderPath_);

        ImGui::PopID();
    }
    ImGui::End();
#endif // _DEBUG
}
