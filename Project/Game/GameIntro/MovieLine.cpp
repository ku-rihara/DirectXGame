#include "MovieLine.h"
#include "base/TextureManager.h"
#include "Frame/Frame.h"
#include <cstdint>
#include <imgui.h>

void MovieLine::Init() {
    /// グローバルパラメータ
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_, false);
    BindParams();
    globalParameter_->SyncParamForGroup(groupName_);

    // create
    for (int32_t i = 0; i < static_cast<int32_t>(sprite_.size()); ++i) {
        sprite_[i].reset(Sprite::Create("movieLine.png"));
    }

    exitEasing_[0].Init("UPMovieLineExit", "UPMovieLineExit.json");
    exitEasing_[1].Init("DownMovieLineExit", "DownMovieLineExit.json");

    appearEasing_[0].Init("UPMovieLineAppear", "UPMovieLineAppear.json");
    appearEasing_[1].Init("DownMovieLineAppear", "DownMovieLineAppear.json");

    for (int32_t i = 0; i < static_cast<int32_t>(exitEasing_.size()); ++i) {
        exitEasing_[i].SetAdaptValue(&exitAdaptPos_[i]);
        exitEasing_[i].SetStartValue(appearPosition_[i].y);
        exitEasing_[i].Reset();
    }

    for (int32_t i = 0; i < static_cast<int32_t>(appearEasing_.size()); ++i) {
        appearEasing_[i].SetAdaptValue(&appearAdaptPos_[i]);
        appearEasing_[i].Reset();
    }
}

void MovieLine::AppearUpdate() {
    for (int32_t i = 0; i < static_cast<int32_t>(appearEasing_.size()); ++i) {
        appearEasing_[i].Update(Frame::DeltaTime());
        sprite_[i]->transform_.pos = appearAdaptPos_[i];
    }
}

void MovieLine::ExitUpdate() {
    for (int32_t i = 0; i < static_cast<int32_t>(exitEasing_.size()); ++i) {
        exitEasing_[i].Update(Frame::DeltaTime());
        sprite_[i]->transform_.pos = Vector2(appearPosition_[i].x, exitAdaptPos_[i].y);
    }
}


///=========================================================
/// バインド
///==========================================================
void MovieLine::BindParams() {

    globalParameter_->Bind(groupName_, "appearPositionUP", &appearPosition_[0]);
    globalParameter_->Bind(groupName_, "appearPositionDown", &appearPosition_[1]);
}
///=========================================================
/// パラメータ調整
///==========================================================
void MovieLine::AdjustParam() {

#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::DragFloat2("appearPosUP", &appearPosition_[0].x, 0.1f);
        ImGui::DragFloat2("appearPosDown", &appearPosition_[1].x, 0.1f);

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
#endif // _DEBUG
}