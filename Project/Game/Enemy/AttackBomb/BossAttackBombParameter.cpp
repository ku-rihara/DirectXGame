#include "BossAttackBombParameter.h"
#include <imgui.h>

void BossAttackBombParameter::Init() {

    // globalParameterの作成
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);
}

void BossAttackBombParameter::RegisterParams() {

    // パラメータ登録
    globalParameter_->Regist(groupName_, "bombThrowInterval", &bombThrowInterval_);
    globalParameter_->Regist(groupName_, "stressPerBomb", &stressPerBomb_);
    globalParameter_->Regist(groupName_, "flightTime", &bombParam_.flightTime);
    globalParameter_->Regist(groupName_, "arcHeight", &bombParam_.arcHeight);
    globalParameter_->Regist(groupName_, "rotateZSpeed", &bombParam_.rotateZSpeed);
    globalParameter_->Regist(groupName_, "scale", &bombParam_.scale);
}

void BossAttackBombParameter::AdjustParam() {

    // パラメータ調整
#if defined(_DEBUG) || defined(DEVELOPMENT)
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::DragFloat("爆弾投げ間隔（秒）", &bombThrowInterval_, 0.1f);
        ImGui::DragFloat("着地時ストレス増加量", &stressPerBomb_, 0.1f);
        ImGui::DragFloat("飛行時間（秒）", &bombParam_.flightTime, 0.05f);
        ImGui::DragFloat("放物線の高さ", &bombParam_.arcHeight, 0.1f);
        ImGui::DragFloat("Z回転速度", &bombParam_.rotateZSpeed, 0.01f);
        ImGui::DragFloat3("スケール", &bombParam_.scale.x, 0.01f);

        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
#endif
}
