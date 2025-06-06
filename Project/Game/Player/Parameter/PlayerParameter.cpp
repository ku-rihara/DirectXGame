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
    AddParmGroup();
    ApplyGlobalParameter();
}

///=========================================================
/// 　更新
///==========================================================

///=========================================================
/// パラメータ調整
///==========================================================
void PlayerParameter::AdjustParm() {
    SetValues();
#ifdef _DEBUG
    if (ImGui::CollapsingHeader("Player")) {

        /// 位置
        ImGui::SeparatorText("Transform");
        ImGui::DragFloat3("Position", &playerParams_.startPos_.x, 0.1f);

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
        ImGui::DragFloat("rushEaseMax", &playerParams_.rushEaseMax, 0.01f, 0);
        ImGui::SliderAngle("attackRotate", &playerParams_.attackRotate, 0, 720);
        ImGui::SliderAngle("attackRotateAnit", &playerParams_.attackRotateAnit, -360, 720);

        ImGui::DragFloat("attackFloatValue", &playerParams_.attackFloatValue, 0.01f);
        ImGui::SeparatorText("Upper");

        ImGui::DragFloat("BackLashValue", &playerParams_.upperParm.BackLashValue, 0.01f);
        ImGui::DragFloat("jumpPowerU", &playerParams_.upperJump.jumpSpeed, 0.01f);
        ImGui::DragFloat("gravityU", &playerParams_.upperJump.gravity, 0.01f);
        ImGui::DragFloat("fallSpeedLimitU", &playerParams_.upperJump.fallSpeedLimit, 0.01f);
        ImGui::DragFloat("UpperPosY", &playerParams_.upperPosY, 0.1f);
        ImGui::SeparatorText("Bound");
        ImGui::DragFloat("jumpPowerB", &playerParams_.bountJump.jumpSpeed, 0.01f);
        ImGui::DragFloat("gravityB", &playerParams_.bountJump.gravity, 0.01f);
        ImGui::DragFloat("fallSpeedLimitB", &playerParams_.bountJump.fallSpeedLimit, 0.01f);
        ImGui::SeparatorText("EasingTime");
        ImGui::DragFloat("BackLashEaseTime", &playerParams_.upperParm.BackLashEaseTime, 0.01f);
        ImGui::DragFloat("attackRotateEaseT", &playerParams_.attackRotateEaseT, 0.01f);
        ImGui::DragFloat("attackFloatEaseT", &playerParams_.attackFloatEaseT, 0.01f);

        /// コンボパラメータ
        if (ImGui::CollapsingHeader("NormalCombo")) {
            ImGui::SeparatorText("FirstCombo"); /// 1コンボ目

            ImGui::DragFloat("PTime1", &normalComboParms_[0].waitTime, 0.01f);
            ImGui::DragFloat("PunchEaseMax1", &normalComboParms_[0].attackEaseMax, 0.01f);
            ImGui::DragFloat("PunchReach1", &normalComboParms_[0].attackReach, 0.01f);

            ImGui::SeparatorText("SecondCombo"); /// 2コンボ目

            ImGui::DragFloat("PTime2", &normalComboParms_[1].waitTime, 0.01f);
            ImGui::DragFloat("PunchEaseMax2", &normalComboParms_[1].attackEaseMax, 0.01f);
            ImGui::DragFloat("PunchReach2", &normalComboParms_[1].attackReach, 0.01f);

            ImGui::SeparatorText("ThirdCombo"); /// 3コンボ目

            ImGui::DragFloat("PTime3", &normalComboParms_[2].waitTime, 0.01f);
            ImGui::DragFloat("PunchEaseMax3", &normalComboParms_[2].attackEaseMax, 0.01f);

            ImGui::SeparatorText("ForthCombo"); /// 4コンボ目

            ImGui::DragFloat("PTime4", &normalComboParms_[3].waitTime, 0.01f);
        }

        if (ImGui::CollapsingHeader("JumpCombo")) {
            ImGui::SeparatorText("FirstCombo"); /// 1コンボ目

            ImGui::DragFloat("JPTime1", &jumpComboParms_[0].waitTime, 0.01f);

            ImGui::DragFloat("JPunchEaseMax1", &jumpComboParms_[0].attackEaseMax, 0.01f);

            ImGui::SeparatorText("SecondCombo"); /// 2コンボ目

            ImGui::DragFloat("JPTime2", &jumpComboParms_[1].waitTime, 0.01f);
            ImGui::DragFloat("JPunchEaseMax2", &jumpComboParms_[1].attackEaseMax, 0.01f);
            ImGui::DragFloat("JPunchReach2", &jumpComboParms_[1].attackReach, 0.01f);
        }

        /// セーブとロード
        globalParameter_->ParamSaveForImGui(groupName_);
        ParamLoadForImGui();
    }

  /*  /// 手のパラメータ調整
    leftHand_->AdjustParm();
    rightHand_->AdjustParm();*/

#endif // _DEBUG
}

///=================================================================================
/// ロード
///=================================================================================
void PlayerParameter::ParamLoadForImGui() {

    // ロードボタン
    if (ImGui::Button(std::format("Load {}", groupName_).c_str())) {

        globalParameter_->LoadFile(groupName_);
        // セーブ完了メッセージ
        ImGui::Text("Load Successful: %s", groupName_.c_str());
        ApplyGlobalParameter();
    }
}

///=================================================================================
/// パラメータをグループに追加
///=================================================================================
void PlayerParameter::AddParmGroup() {

    globalParameter_->AddItem(groupName_, "Translate", playerParams_.startPos_);
    globalParameter_->AddItem(groupName_, "JumpSpeed", playerParams_.normalJump.jumpSpeed);
    globalParameter_->AddItem(groupName_, "rushDistance", playerParams_.rushDistance);
    globalParameter_->AddItem(groupName_, "UpperPosY", playerParams_.upperPosY);
    globalParameter_->AddItem(groupName_, "MoveSpeed", playerParams_.moveSpeed);
    globalParameter_->AddItem(groupName_, "Gravity", playerParams_.normalJump.gravity);
    globalParameter_->AddItem(groupName_, "FallSpeed", playerParams_.fallSpeed);
    globalParameter_->AddItem(groupName_, "FallSpeedLimit", playerParams_.normalJump.fallSpeedLimit);
    globalParameter_->AddItem(groupName_, "attackRotate", playerParams_.attackRotate);
    globalParameter_->AddItem(groupName_, "attackFloatValue_", playerParams_.attackFloatValue);
    globalParameter_->AddItem(groupName_, "upperBackLashValue_", playerParams_.upperParm.BackLashValue);
    globalParameter_->AddItem(groupName_, "jumpPowerU", playerParams_.upperJump.jumpSpeed);
    globalParameter_->AddItem(groupName_, "gravityU", playerParams_.upperJump.gravity);
    globalParameter_->AddItem(groupName_, "fallSpeedLimitU", playerParams_.upperJump.fallSpeedLimit);
    globalParameter_->AddItem(groupName_, "jumpSpeedB", playerParams_.bountJump.jumpSpeed);
    globalParameter_->AddItem(groupName_, "gravityB", playerParams_.bountJump.gravity);
    globalParameter_->AddItem(groupName_, "fallSpeedLimitB", playerParams_.bountJump.fallSpeedLimit);
    globalParameter_->AddItem(groupName_, "attackRotateAnit", playerParams_.attackRotateAnit);

    /// コンボ持続時間
    for (uint32_t i = 0; i < normalComboParms_.size(); ++i) {
        globalParameter_->AddItem(groupName_, "NComboPTime" + std::to_string(int(i + 1)), normalComboParms_[i].waitTime);
        globalParameter_->AddItem(groupName_, "NComboPunchEaseTime" + std::to_string(int(i + 1)), normalComboParms_[i].attackEaseMax);
        globalParameter_->AddItem(groupName_, "NComboPunchReach" + std::to_string(int(i + 1)), normalComboParms_[i].attackReach);
    }

    /// コンボ持続時間(ジャンプ)
    for (uint32_t i = 0; i < jumpComboParms_.size(); ++i) {
        globalParameter_->AddItem(groupName_, "JComboPTime" + std::to_string(int(i + 1)), normalComboParms_[i].waitTime);
        globalParameter_->AddItem(groupName_, "JComboPunchEaseTime" + std::to_string(int(i + 1)), normalComboParms_[i].attackEaseMax);
        globalParameter_->AddItem(groupName_, "JComboPunchReach" + std::to_string(int(i + 1)), normalComboParms_[i].attackReach);
    }

    globalParameter_->AddItem(groupName_, "rushEaseMax", playerParams_.rushEaseMax);
    globalParameter_->AddItem(groupName_, "upperBackLashEaseTime_", playerParams_.upperParm.BackLashEaseTime);
    globalParameter_->AddItem(groupName_, "attackRotateEaseT", playerParams_.attackRotateEaseT);
    globalParameter_->AddItem(groupName_, "attackFloatEaseT_", playerParams_.attackFloatEaseT);
}

///=================================================================================
/// パラメータをグループに追加
///=================================================================================
void PlayerParameter::SetValues() {

    globalParameter_->SetValue(groupName_, "Translate", playerParams_.startPos_);
    globalParameter_->SetValue(groupName_, "JumpSpeed", playerParams_.normalJump.jumpSpeed);
    globalParameter_->SetValue(groupName_, "rushDistance", playerParams_.rushDistance);
    globalParameter_->SetValue(groupName_, "UpperPosY", playerParams_.upperPosY);
    globalParameter_->SetValue(groupName_, "MoveSpeed", playerParams_.moveSpeed);
    globalParameter_->SetValue(groupName_, "Gravity", playerParams_.normalJump.gravity);
    globalParameter_->SetValue(groupName_, "FallSpeed", playerParams_.fallSpeed);
    globalParameter_->SetValue(groupName_, "FallSpeedLimit", playerParams_.normalJump.fallSpeedLimit);
    globalParameter_->SetValue(groupName_, "attackRotate", playerParams_.attackRotate);
    globalParameter_->SetValue(groupName_, "attackFloatValue_", playerParams_.attackFloatValue);
    globalParameter_->SetValue(groupName_, "upperBackLashValue_", playerParams_.upperParm.BackLashValue);
    globalParameter_->SetValue(groupName_, "jumpPowerU", playerParams_.upperJump.jumpSpeed);
    globalParameter_->SetValue(groupName_, "gravityU", playerParams_.upperJump.gravity);
    globalParameter_->SetValue(groupName_, "fallSpeedLimitU", playerParams_.upperJump.fallSpeedLimit);
    globalParameter_->SetValue(groupName_, "jumpSpeedB", playerParams_.bountJump.jumpSpeed);
    globalParameter_->SetValue(groupName_, "gravityB", playerParams_.bountJump.gravity);
    globalParameter_->SetValue(groupName_, "fallSpeedLimitB", playerParams_.bountJump.fallSpeedLimit);
    globalParameter_->SetValue(groupName_, "attackRotateAnit", playerParams_.attackRotateAnit);

    /// コンボ持続時間
    for (uint32_t i = 0; i < normalComboParms_.size(); ++i) {
        globalParameter_->SetValue(groupName_, "NComboPTime" + std::to_string(int(i + 1)), normalComboParms_[i].waitTime);
        globalParameter_->SetValue(groupName_, "NComboPunchEaseTime" + std::to_string(int(i + 1)), normalComboParms_[i].attackEaseMax);
        globalParameter_->SetValue(groupName_, "NComboPunchReach" + std::to_string(int(i + 1)), normalComboParms_[i].attackReach);
    }

    /// コンボ持続時間(ジャンプ)
    for (uint32_t i = 0; i < jumpComboParms_.size(); ++i) {
        globalParameter_->SetValue(groupName_, "JComboPTime" + std::to_string(int(i + 1)), normalComboParms_[i].waitTime);
        globalParameter_->SetValue(groupName_, "JComboPunchEaseTime" + std::to_string(int(i + 1)), normalComboParms_[i].attackEaseMax);
        globalParameter_->SetValue(groupName_, "JComboPunchReach" + std::to_string(int(i + 1)), normalComboParms_[i].attackReach);
    }

    globalParameter_->SetValue(groupName_, "rushEaseMax", playerParams_.rushEaseMax);
    globalParameter_->SetValue(groupName_, "upperBackLashEaseTime_", playerParams_.upperParm.BackLashEaseTime);
    globalParameter_->SetValue(groupName_, "attackRotateEaseT", playerParams_.attackRotateEaseT);
    globalParameter_->SetValue(groupName_, "attackFloatEaseT_", playerParams_.attackFloatEaseT);
}

///=====================================================
///  ImGuiからパラメータを得る
///=====================================================
void PlayerParameter::ApplyGlobalParameter() {

    playerParams_.startPos_                 = globalParameter_->GetValue<Vector3>(groupName_, "Translate");
    playerParams_.normalJump.jumpSpeed      = globalParameter_->GetValue<float>(groupName_, "JumpSpeed");
    playerParams_.rushDistance              = globalParameter_->GetValue<float>(groupName_, "rushDistance");
    playerParams_.rushEaseMax               = globalParameter_->GetValue<float>(groupName_, "rushEaseMax");
    playerParams_.upperPosY                 = globalParameter_->GetValue<float>(groupName_, "UpperPosY");
    playerParams_.moveSpeed                 = globalParameter_->GetValue<float>(groupName_, "MoveSpeed");
    playerParams_.normalJump.gravity        = globalParameter_->GetValue<float>(groupName_, "Gravity");
    playerParams_.fallSpeed                 = globalParameter_->GetValue<float>(groupName_, "FallSpeed");
    playerParams_.normalJump.fallSpeedLimit = globalParameter_->GetValue<float>(groupName_, "FallSpeedLimit");
    playerParams_.attackRotateEaseT         = globalParameter_->GetValue<float>(groupName_, "attackRotateEaseT");
    playerParams_.attackRotate              = globalParameter_->GetValue<float>(groupName_, "attackRotate");
    playerParams_.attackFloatEaseT          = globalParameter_->GetValue<float>(groupName_, "attackFloatEaseT_");
    playerParams_.attackFloatValue          = globalParameter_->GetValue<float>(groupName_, "attackFloatValue_");
    playerParams_.attackRotateAnit          = globalParameter_->GetValue<float>(groupName_, "attackRotateAnit");

    playerParams_.upperParm.BackLashValue    = globalParameter_->GetValue<float>(groupName_, "upperBackLashValue_");
    playerParams_.upperParm.BackLashEaseTime = globalParameter_->GetValue<float>(groupName_, "upperBackLashEaseTime_");
    playerParams_.upperJump.jumpSpeed        = globalParameter_->GetValue<float>(groupName_, "jumpPowerU");

    playerParams_.bountJump.fallSpeedLimit = globalParameter_->GetValue<float>(groupName_, "fallSpeedLimitB");
    playerParams_.bountJump.gravity        = globalParameter_->GetValue<float>(groupName_, "gravityB");
    playerParams_.bountJump.jumpSpeed      = globalParameter_->GetValue<float>(groupName_, "jumpSpeedB");

    playerParams_.upperJump.fallSpeedLimit = globalParameter_->GetValue<float>(groupName_, "fallSpeedLimitU");
    playerParams_.upperJump.gravity        = globalParameter_->GetValue<float>(groupName_, "gravityU");
    playerParams_.upperJump.jumpSpeed      = globalParameter_->GetValue<float>(groupName_, "jumpPowerU");

    /// コンボ持続時間
    for (uint32_t i = 0; i < normalComboParms_.size(); ++i) {
        normalComboParms_[i].waitTime      = globalParameter_->GetValue<float>(groupName_, "NComboPTime" + std::to_string(int(i + 1)));
        normalComboParms_[i].attackEaseMax = globalParameter_->GetValue<float>(groupName_, "NComboPunchEaseTime" + std::to_string(int(i + 1)));
        normalComboParms_[i].attackReach   = globalParameter_->GetValue<float>(groupName_, "NComboPunchReach" + std::to_string(int(i + 1)));
    }

    /// コンボ持続時間
    for (uint32_t i = 0; i < jumpComboParms_.size(); ++i) {
        jumpComboParms_[i].waitTime      = globalParameter_->GetValue<float>(groupName_, "JComboPTime" + std::to_string(int(i + 1)));
        jumpComboParms_[i].attackEaseMax = globalParameter_->GetValue<float>(groupName_, "JComboPunchEaseTime" + std::to_string(int(i + 1)));
        jumpComboParms_[i].attackReach   = globalParameter_->GetValue<float>(groupName_, "JComboPunchReach" + std::to_string(int(i + 1)));
    }
}
