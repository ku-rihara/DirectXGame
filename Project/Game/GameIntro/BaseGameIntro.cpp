#include"BaseGameIntro.h"
#include<imgui.h>

void BaseGameIntro::Init(const std::string& name) {
    groupName_ = name;

    // Global parameter 
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_, false);
    BindParams();
    globalParameter_->SyncParamForGroup(groupName_);
}

void BaseGameIntro::Update(const float& playSpeed) {
    playSpeed_ = playSpeed;
}


void BaseGameIntro::BindParams() {
   
}

void BaseGameIntro::AdjustParam() {

#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

       
        AdjustUniqueParam();

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
#endif // _DEBUG
}