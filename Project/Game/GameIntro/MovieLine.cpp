#include "MovieLine.h"
#include "base/TextureManager.h"
#include <cstdint>
#include <imgui.h>

void MovieLine::Init() {
    /// グローバルパラメータ
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_, false);
    BindParams();
    globalParameter_->SyncParamForGroup(groupName_);

    int32_t textureHandle = TextureManager::GetInstance()->LoadTexture("Resources/Texture/movieLine.png");

    // create
    for (int32_t i = 0; i < static_cast<int32_t>(sprite_.size()); ++i) {
        sprite_[i].reset(Sprite::Create(textureHandle, initPosition_[i], Vector4::kWHITE()));
    }
}

void MovieLine::Update() {
}

void MovieLine::Draw() {
}

///=========================================================
/// バインド
///==========================================================
void MovieLine::BindParams() {

    globalParameter_->Bind(groupName_, "initPositionUP", &initPosition_[0]);
    globalParameter_->Bind(groupName_, "initPositionDown", &initPosition_[1]);
}
///=========================================================
/// パラメータ調整
///==========================================================
void MovieLine::AdjustParam() {

#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::DragFloat2("InitPosUP", &initPosition_[0].x, 0.1f);
        ImGui::DragFloat2("InitPosDown", &initPosition_[1].x, 0.1f);

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
#endif // _DEBUG
}