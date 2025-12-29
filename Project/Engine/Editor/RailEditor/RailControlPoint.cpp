#include "RailControlPoint.h"

using namespace KetaEngine;
#include <imgui.h>

void RailControlPoint::Init(const std::string& railName, int32_t keyNumber) {
    // グローバルパラメータ
    globalParameter_         = GlobalParameter::GetInstance();
    currenTSequenceElementIndex     = keyNumber;
    std::string newGroupName    = railName + std::to_string(currenTSequenceElementIndex);
    groupName_               = newGroupName;
    folderPath_ += railName;

    if (!globalParameter_->HasRegisters(groupName_)) {
        // 新規登録
        globalParameter_->CreateGroup(groupName_);
        RegisterParams();
    } else {
        // パラメータを取得
        GetParams();
    }
}

void RailControlPoint::Reset() {
    position_ = {0.0f, 0.0f, 0.0f};
}

void RailControlPoint::LoadData() {
    // パラメータファイルから読み込み
    globalParameter_->LoadFile(groupName_, folderPath_);
    globalParameter_->SyncParamForGroup(groupName_);
}

void RailControlPoint::SaveData() {
    // パラメータファイルに保存
    globalParameter_->SaveFile(groupName_, folderPath_);
}

void RailControlPoint::RegisterParams() {
    globalParameter_->Regist(groupName_, "position", &position_);
}

void RailControlPoint::GetParams() {
    position_ = globalParameter_->GetValue<Vector3>(groupName_, "position");
}

void RailControlPoint::AdjustParam() {
#ifdef _DEBUG
    ImGui::SeparatorText(("Rail KeyFrame: " + groupName_).c_str());
    ImGui::PushID(groupName_.c_str());

    ImGui::DragFloat3("Position", &position_.x, 0.1f);

    ImGui::PopID();
#endif // _DEBUG
}
