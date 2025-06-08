#include "ComboUI.h"
#include "Easing.h"
#include"base/TextureManager.h"
#include <imgui.h>
#include <cstdint>

void ComboUI::Init() {
    //int32_t textureId = TextureManager::GetInstance()->LoadTexture();

    //// Sprite create
    //sprite_.reset(Sprite::Create())

    ///* グローバルパラメータ
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_, false);
    BindParams();
    globalParameter_->SyncGroupFromUI(groupName_);
}

void ComboUI::Update() {
}
void ComboUI::Draw() {
}



///=========================================================
/// パラメータ調整
///==========================================================
void ComboUI::AdjustParm() {
   
#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
      

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);
       
        ImGui::PopID();
    }
#endif // _DEBUG
}

void ComboUI::BindParams() {
  
}


