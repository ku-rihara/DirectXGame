#include "EnemyHPBarColorConfig.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include <imgui.h>

void EnemyHPBarColorConfig::Init() {
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->LoadFile(groupName_, "Application");
    globalParameter_->SyncParamForGroup(groupName_);
}

void EnemyHPBarColorConfig::RegisterParams() {
    const std::array<std::string, 3> labels = {"High", "Mid", "Low"};
    for (int i = 0; i < 3; ++i) {
        globalParameter_->Regist(groupName_, "Stage" + labels[i] + "_Threshold", &stages_[i].threshold);
        globalParameter_->Regist(groupName_, "Stage" + labels[i] + "_Color",     &stages_[i].color);
    }
}

void EnemyHPBarColorConfig::AdjustParam() {
#ifdef _DEBUG
    ImGui::SeparatorText("HPバー色設定（3段階）");
    const std::array<const char*, 3> labels = {"高HP", "中HP", "低HP"};
    for (int i = 0; i < 3; ++i) {
        ImGui::PushID(i);
        ImGui::Text("%s", labels[i]);
        ImGui::SliderFloat("閾値(以上)", &stages_[i].threshold, 0.0f, 1.0f);
        ImGui::ColorEdit3("色", &stages_[i].color.x);
        ImGui::PopID();
    }
    globalParameter_->ParamSaveForImGui(groupName_);
    globalParameter_->ParamLoadForImGui(groupName_);
#endif
}

Vector3 EnemyHPBarColorConfig::GetColor(float ratio) const {
    for (const auto& stage : stages_) {
        if (ratio >= stage.threshold) {
            return stage.color;
        }
    }
    return stages_[2].color;
}
