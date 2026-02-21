#include "PlayerParameter.h"
// imGui
#include <imgui.h>

///==========================================================
/// 　初期化
///==========================================================
void PlayerParameter::Init() {

    // グローバルパラメータ
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);
}

///==========================================================
/// パラメータ調整
///==========================================================
void PlayerParameter::AdjustParam() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        /// 位置
        ImGui::SeparatorText("Transform");
        ImGui::DragFloat3("Position", &playerParams_.startPos_.x, 0.1f);
        ImGui::DragFloat3("BaseScale", &playerParams_.baseScale_.x, 0.01f);

        ImGui::SeparatorText("SpawnParam");
        ImGui::DragFloat("spawnParamWaitTime", &playerParams_.spawnParam.waitTime_, 0.01f);

        /// 　Floatのパラメータ
        ImGui::SeparatorText("FloatParameter");
        ImGui::SeparatorText("Normal");
        ImGui::DragFloat("jumpSpeed", &playerParams_.normalJump.jumpSpeed, 0.01f);
        ImGui::DragFloat("MoveSpeed", &playerParams_.moveSpeed, 0.01f);
        ImGui::DragFloat("DashSpeedMultiplier", &playerParams_.dashSpeedMultiplier, 0.01f);
        ImGui::DragFloat("Gravity", &playerParams_.normalJump.gravity, 0.01f);
        ImGui::DragFloat("FallSpeed", &playerParams_.fallSpeed, 0.01f);
        ImGui::DragFloat("fallSpeedLimit", &playerParams_.normalJump.fallSpeedLimit, 0.1f);
        ImGui::SeparatorText("FurstPunch");
        ImGui::DragFloat("rushDistance", &playerParams_.rushDistance, 0.01f);
        ImGui::SliderAngle("attackRotate", &playerParams_.attackRotate, 0, 720);
        ImGui::SliderAngle("attackRotateAnit", &playerParams_.attackRotateAnit, -360, 720);

        ImGui::DragFloat("attackFloatValue", &playerParams_.attackFloatValue, 0.01f);
        ImGui::SeparatorText("Upper");

        ImGui::DragFloat("BackLashValue", &playerParams_.upperParam.BackLashValue, 0.01f);
        ImGui::DragFloat("UpperChargeTime", &playerParams_.upperParam.chargeTime, 0.01f);
        ImGui::DragFloat("jumpPowerU", &playerParams_.upperJump.jumpSpeed, 0.01f);
        ImGui::DragFloat("gravityU", &playerParams_.upperJump.gravity, 0.01f);
        ImGui::DragFloat("fallSpeedLimitU", &playerParams_.upperJump.fallSpeedLimit, 0.01f);
        ImGui::DragFloat("UpperPosY", &playerParams_.upperPosY, 0.1f);
        ImGui::SeparatorText("Bound");
        ImGui::DragFloat("jumpPowerB", &playerParams_.boundJump.jumpSpeed, 0.01f);
        ImGui::DragFloat("gravityB", &playerParams_.boundJump.gravity, 0.01f);
        ImGui::DragFloat("fallSpeedLimitB", &playerParams_.boundJump.fallSpeedLimit, 0.01f);


        ImGui::SeparatorText("EasingTime");

      
        /// セーブとロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }

#endif // _DEBUG
}

///=====================================================
///  ImGuiからパラメータを得る
///=====================================================
void PlayerParameter::RegisterParams() {

    globalParameter_->Regist(groupName_, "Translate", &playerParams_.startPos_);
    globalParameter_->Regist(groupName_, "BaseScale", &playerParams_.baseScale_);
    globalParameter_->Regist(groupName_, "JumpSpeed", &playerParams_.normalJump.jumpSpeed);
    globalParameter_->Regist(groupName_, "rushDistance", &playerParams_.rushDistance);
    globalParameter_->Regist(groupName_, "UpperPosY", &playerParams_.upperPosY);
    globalParameter_->Regist(groupName_, "MoveSpeed", &playerParams_.moveSpeed);
    globalParameter_->Regist(groupName_, "DashSpeedMultiplier", &playerParams_.dashSpeedMultiplier);
    globalParameter_->Regist(groupName_, "Gravity", &playerParams_.normalJump.gravity);
    globalParameter_->Regist(groupName_, "FallSpeed", &playerParams_.fallSpeed);
    globalParameter_->Regist(groupName_, "FallSpeedLimit", &playerParams_.normalJump.fallSpeedLimit);
    globalParameter_->Regist(groupName_, "attackRotate", &playerParams_.attackRotate);
    globalParameter_->Regist(groupName_, "attackFloatValue_", &playerParams_.attackFloatValue);
    globalParameter_->Regist(groupName_, "attackRotateAnit", &playerParams_.attackRotateAnit);

    globalParameter_->Regist(groupName_, "upperBackLashValue_", &playerParams_.upperParam.BackLashValue);
    globalParameter_->Regist(groupName_, "jumpPowerU", &playerParams_.upperJump.jumpSpeed);

    globalParameter_->Regist(groupName_, "fallSpeedLimitB", &playerParams_.boundJump.fallSpeedLimit);
    globalParameter_->Regist(groupName_, "gravityB", &playerParams_.boundJump.gravity);
    globalParameter_->Regist(groupName_, "jumpSpeedB", &playerParams_.boundJump.jumpSpeed);

    globalParameter_->Regist(groupName_, "fallSpeedLimitU", &playerParams_.upperJump.fallSpeedLimit);
    globalParameter_->Regist(groupName_, "gravityU", &playerParams_.upperJump.gravity);
    globalParameter_->Regist(groupName_, "jumpPowerU", &playerParams_.upperJump.jumpSpeed);
    globalParameter_->Regist(groupName_, "UpperChargeTime", &playerParams_.upperParam.chargeTime);

   
    globalParameter_->Regist(groupName_, "spawnParamWaitTime", &playerParams_.spawnParam.waitTime_);
}