#include "Ground.h"
#include <imgui.h>

using namespace KetaEngine;

void Ground::Init() {
    // モデル生成
    obj3d_.reset(Object3d::CreateModel(kModelName_));

    // 原点に配置
    obj3d_->transform_.translation_ = Vector3();
    obj3d_->transform_.rotation_    = Vector3();
    obj3d_->transform_.scale_       = size_;

    // グローバルパラメータに登録
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(kGroupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(kGroupName_);
}

void Ground::RegisterParams() {
    globalParameter_->Regist(kGroupName_, "Size", &size_);
}

void Ground::Update() {
    if (!obj3d_) {
        return;
    }

    // ImGuiや保存データで変更されたサイズを反映する
    obj3d_->transform_.scale_ = size_;
}

void Ground::AdjustParam() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    if (ImGui::CollapsingHeader(kGroupName_.c_str())) {
        ImGui::PushID(kGroupName_.c_str());

        ImGui::DragFloat3("Size", &size_.x, 0.01f, 0.0f, 1000.0f);

        // 登録が切れてたら再登録
        if (!globalParameter_->HasRegisters(kGroupName_)) {
            RegisterParams();
        }

        globalParameter_->ParamSaveForImGui(kGroupName_);
        globalParameter_->ParamLoadForImGui(kGroupName_);

        ImGui::PopID();
    }
#endif
}
