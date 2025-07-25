#include "PlayerParameter.h"
/// frame
#include "Frame/Frame.h"
/// imgui
#include <imgui.h>

PlayerParameter::PlayerParameter() {}

///=========================================================
/// 　初期化
///==========================================================
void PlayerParameter::Init() {

    ///* グローバルパラメータ
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_, false);
    BindParams();
    globalParameter_->SyncParamForGroup(groupName_);
}

///=========================================================
/// 　更新
///==========================================================

///=========================================================
/// パラメータ調整
///==========================================================
void PlayerParameter::AdjustParam() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader("Player")) {

        /// 位置
        ImGui::SeparatorText("Transform");
        ImGui::DragFloat3("Position", &playerParams_.startPos_.x, 0.1f);

         ImGui::SeparatorText("SpawnParam");
        ImGui::DragFloat("spawnParamWaitTime", &playerParams_.spawnParam.waitTime_, 0.01f);

        /// 　Floatのパラメータ
        ImGui::SeparatorText("FloatParamater");
        ImGui::SeparatorText("Normal");
        ImGui::DragFloat("jumpSpeed", &playerParams_.normalJump.jumpSpeed, 0.01f);
        ImGui::DragFloat("MoveSpeed", &playerParams_.moveSpeed, 0.01f);
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
      
        /// コンボパラメータ
        if (ImGui::CollapsingHeader("NormalCombo")) {
            ImGui::SeparatorText("FirstCombo"); /// 1コンボ目

            ImGui::DragFloat("PTime1", &normalComboParams_[0].waitTime, 0.01f);
            ImGui::DragFloat("PunchReach1", &normalComboParams_[0].attackReach, 0.01f);

            ImGui::SeparatorText("SecondCombo"); /// 2コンボ目

            ImGui::DragFloat("PTime2", &normalComboParams_[1].waitTime, 0.01f);
            ImGui::DragFloat("PunchReach2", &normalComboParams_[1].attackReach, 0.01f);

            ImGui::SeparatorText("ThirdCombo"); /// 3コンボ目

            ImGui::DragFloat("PTime3", &normalComboParams_[2].waitTime, 0.01f);

            ImGui::SeparatorText("ForthCombo"); /// 4コンボ目

            ImGui::DragFloat("PTime4", &normalComboParams_[3].waitTime, 0.01f);
        }

        if (ImGui::CollapsingHeader("JumpCombo")) {
            ImGui::SeparatorText("FirstCombo"); /// 1コンボ目

            ImGui::DragFloat("JPTime1", &jumpComboParams_[0].waitTime, 0.01f);

            ImGui::SeparatorText("SecondCombo"); /// 2コンボ目

            ImGui::DragFloat("JPTime2", &jumpComboParams_[1].waitTime, 0.01f);
            ImGui::DragFloat("JPunchReach2", &jumpComboParams_[1].attackReach, 0.01f);
        }

        /// セーブとロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);
    }

#endif // _DEBUG
}

///=====================================================
///  ImGuiからパラメータを得る
///=====================================================
void PlayerParameter::BindParams() {

    globalParameter_->Bind(groupName_, "Translate", &playerParams_.startPos_);
    globalParameter_->Bind(groupName_, "JumpSpeed", &playerParams_.normalJump.jumpSpeed);
    globalParameter_->Bind(groupName_, "rushDistance", &playerParams_.rushDistance);
    globalParameter_->Bind(groupName_, "UpperPosY", &playerParams_.upperPosY);
    globalParameter_->Bind(groupName_, "MoveSpeed", &playerParams_.moveSpeed);
    globalParameter_->Bind(groupName_, "Gravity", &playerParams_.normalJump.gravity);
    globalParameter_->Bind(groupName_, "FallSpeed", &playerParams_.fallSpeed);
    globalParameter_->Bind(groupName_, "FallSpeedLimit", &playerParams_.normalJump.fallSpeedLimit);
    globalParameter_->Bind(groupName_, "attackRotate", &playerParams_.attackRotate);
    globalParameter_->Bind(groupName_, "attackFloatValue_", &playerParams_.attackFloatValue);
    globalParameter_->Bind(groupName_, "attackRotateAnit", &playerParams_.attackRotateAnit);

    globalParameter_->Bind(groupName_, "upperBackLashValue_", &playerParams_.upperParam.BackLashValue);
    globalParameter_->Bind(groupName_, "jumpPowerU", &playerParams_.upperJump.jumpSpeed);

    globalParameter_->Bind(groupName_, "fallSpeedLimitB", &playerParams_.boundJump.fallSpeedLimit);
    globalParameter_->Bind(groupName_, "gravityB", &playerParams_.boundJump.gravity);
    globalParameter_->Bind(groupName_, "jumpSpeedB", &playerParams_.boundJump.jumpSpeed);
    
    globalParameter_->Bind(groupName_, "fallSpeedLimitU", &playerParams_.upperJump.fallSpeedLimit);
    globalParameter_->Bind(groupName_, "gravityU", &playerParams_.upperJump.gravity);
    globalParameter_->Bind(groupName_, "jumpPowerU", &playerParams_.upperJump.jumpSpeed);
    globalParameter_->Bind(groupName_, "UpperChargeTime", &playerParams_.upperParam.chargeTime);

    /// コンボ持続時間
    for (uint32_t i = 0; i < normalComboParams_.size(); ++i) {
        globalParameter_->Bind(groupName_, "NComboPTime" + std::to_string(int(i + 1)), &normalComboParams_[i].waitTime);
         globalParameter_->Bind(groupName_, "NComboPunchReach" + std::to_string(int(i + 1)), &normalComboParams_[i].attackReach);
    }

    /// コンボ持続時間
    for (uint32_t i = 0; i < jumpComboParams_.size(); ++i) {
        globalParameter_->Bind(groupName_, "JComboPTime" + std::to_string(int(i + 1)), &jumpComboParams_[i].waitTime);
         globalParameter_->Bind(groupName_, "JComboPunchReach" + std::to_string(int(i + 1)), &jumpComboParams_[i].attackReach);
    }

    globalParameter_->Bind(groupName_, "spawnParamWaitTime", &playerParams_.spawnParam.waitTime_);
}