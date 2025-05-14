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
        ImGui::DragFloat3("Position", &startPos_.x, 0.1f);

        /// 　Floatのパラメータ
        ImGui::SeparatorText("FloatParamater");
        ImGui::SeparatorText("Normal");
        ImGui::DragFloat("jumpSpeed", &normalJump.jumpSpeed, 0.01f);
        ImGui::DragFloat("MoveSpeed", &moveSpeed, 0.01f);
        ImGui::DragFloat("Gravity", &normalJump.gravity, 0.01f);
        ImGui::DragFloat("FallSpeed", &fallSpeed, 0.01f);
        ImGui::DragFloat("fallSpeedLimit", &normalJump.fallSpeedLimit, 0.1f);
        ImGui::SeparatorText("FurstPunch");
        ImGui::DragFloat("rushDistance", &rushDistance, 0.01f);
        ImGui::DragFloat("rushEaseMax", &rushEaseMax, 0.01f, 0);
        ImGui::SliderAngle("attackRotate", &attackRotate, 0, 720);
        ImGui::SliderAngle("attackRotateAnit", &attackRotateAnit, -360, 720);

        ImGui::DragFloat("attackFloatValue", &attackFloatValue, 0.01f);
        ImGui::SeparatorText("Upper");

        ImGui::DragFloat("BackLashValue", &upperParm.BackLashValue, 0.01f);
        ImGui::DragFloat("jumpPowerU", &upperJump.jumpSpeed, 0.01f);
        ImGui::DragFloat("gravityU", &upperJump.gravity, 0.01f);
        ImGui::DragFloat("fallSpeedLimitU", &upperJump.fallSpeedLimit, 0.01f);
        ImGui::DragFloat("UpperPosY", &upperPosY, 0.1f);
        ImGui::SeparatorText("Bound");
        ImGui::DragFloat("jumpPowerB", &bountJump.jumpSpeed, 0.01f);
        ImGui::DragFloat("gravityB", &bountJump.gravity, 0.01f);
        ImGui::DragFloat("fallSpeedLimitB", &bountJump.fallSpeedLimit, 0.01f);
        ImGui::SeparatorText("EasingTime");
        ImGui::DragFloat("BackLashEaseTime", &upperParm.BackLashEaseTime, 0.01f);
        ImGui::DragFloat("attackRotateEaseT", &attackRotateEaseT, 0.01f);
        ImGui::DragFloat("attackFloatEaseT", &attackFloatEaseT, 0.01f);

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
        globalParameter_->ParmSaveForImGui(groupName_);
        ParmLoadForImGui();
    }

  /*  /// 手のパラメータ調整
    leftHand_->AdjustParm();
    rightHand_->AdjustParm();*/

#endif // _DEBUG
}

///=================================================================================
/// ロード
///=================================================================================
void PlayerParameter::ParmLoadForImGui() {

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

    globalParameter_->AddItem(groupName_, "Translate", startPos_);
    globalParameter_->AddItem(groupName_, "JumpSpeed", normalJump.jumpSpeed);
    globalParameter_->AddItem(groupName_, "rushDistance", rushDistance);
    globalParameter_->AddItem(groupName_, "UpperPosY", upperPosY);
    globalParameter_->AddItem(groupName_, "MoveSpeed", moveSpeed);
    globalParameter_->AddItem(groupName_, "Gravity", normalJump.gravity);
    globalParameter_->AddItem(groupName_, "FallSpeed", fallSpeed);
    globalParameter_->AddItem(groupName_, "FallSpeedLimit", normalJump.fallSpeedLimit);
    globalParameter_->AddItem(groupName_, "attackRotate", attackRotate);
    globalParameter_->AddItem(groupName_, "attackFloatValue_", attackFloatValue);
    globalParameter_->AddItem(groupName_, "upperBackLashValue_", upperParm.BackLashValue);
    globalParameter_->AddItem(groupName_, "jumpPowerU", upperJump.jumpSpeed);
    globalParameter_->AddItem(groupName_, "gravityU", upperJump.gravity);
    globalParameter_->AddItem(groupName_, "fallSpeedLimitU", upperJump.fallSpeedLimit);
    globalParameter_->AddItem(groupName_, "jumpSpeedB", bountJump.jumpSpeed);
    globalParameter_->AddItem(groupName_, "gravityB", bountJump.gravity);
    globalParameter_->AddItem(groupName_, "fallSpeedLimitB", bountJump.fallSpeedLimit);
    globalParameter_->AddItem(groupName_, "attackRotateAnit", attackRotateAnit);

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

    globalParameter_->AddItem(groupName_, "rushEaseMax", rushEaseMax);
    globalParameter_->AddItem(groupName_, "upperBackLashEaseTime_", upperParm.BackLashEaseTime);
    globalParameter_->AddItem(groupName_, "attackRotateEaseT", attackRotateEaseT);
    globalParameter_->AddItem(groupName_, "attackFloatEaseT_", attackFloatEaseT);
}

///=================================================================================
/// パラメータをグループに追加
///=================================================================================
void PlayerParameter::SetValues() {

    globalParameter_->SetValue(groupName_, "Translate", startPos_);
    globalParameter_->SetValue(groupName_, "JumpSpeed", normalJump.jumpSpeed);
    globalParameter_->SetValue(groupName_, "rushDistance", rushDistance);
    globalParameter_->SetValue(groupName_, "UpperPosY", upperPosY);
    globalParameter_->SetValue(groupName_, "MoveSpeed", moveSpeed);
    globalParameter_->SetValue(groupName_, "Gravity", normalJump.gravity);
    globalParameter_->SetValue(groupName_, "FallSpeed", fallSpeed);
    globalParameter_->SetValue(groupName_, "FallSpeedLimit", normalJump.fallSpeedLimit);
    globalParameter_->SetValue(groupName_, "attackRotate", attackRotate);
    globalParameter_->SetValue(groupName_, "attackFloatValue_", attackFloatValue);
    globalParameter_->SetValue(groupName_, "upperBackLashValue_", upperParm.BackLashValue);
    globalParameter_->SetValue(groupName_, "jumpPowerU", upperJump.jumpSpeed);
    globalParameter_->SetValue(groupName_, "gravityU", upperJump.gravity);
    globalParameter_->SetValue(groupName_, "fallSpeedLimitU", upperJump.fallSpeedLimit);
    globalParameter_->SetValue(groupName_, "jumpSpeedB", bountJump.jumpSpeed);
    globalParameter_->SetValue(groupName_, "gravityB", bountJump.gravity);
    globalParameter_->SetValue(groupName_, "fallSpeedLimitB", bountJump.fallSpeedLimit);
    globalParameter_->SetValue(groupName_, "attackRotateAnit", attackRotateAnit);

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

    globalParameter_->SetValue(groupName_, "rushEaseMax", rushEaseMax);
    globalParameter_->SetValue(groupName_, "upperBackLashEaseTime_", upperParm.BackLashEaseTime);
    globalParameter_->SetValue(groupName_, "attackRotateEaseT", attackRotateEaseT);
    globalParameter_->SetValue(groupName_, "attackFloatEaseT_", attackFloatEaseT);
}

///=====================================================
///  ImGuiからパラメータを得る
///=====================================================
void PlayerParameter::ApplyGlobalParameter() {

    startPos_                 = globalParameter_->GetValue<Vector3>(groupName_, "Translate");
    normalJump.jumpSpeed      = globalParameter_->GetValue<float>(groupName_, "JumpSpeed");
    rushDistance              = globalParameter_->GetValue<float>(groupName_, "rushDistance");
    rushEaseMax               = globalParameter_->GetValue<float>(groupName_, "rushEaseMax");
    upperPosY                 = globalParameter_->GetValue<float>(groupName_, "UpperPosY");
    moveSpeed                 = globalParameter_->GetValue<float>(groupName_, "MoveSpeed");
    normalJump.gravity        = globalParameter_->GetValue<float>(groupName_, "Gravity");
    fallSpeed                 = globalParameter_->GetValue<float>(groupName_, "FallSpeed");
    normalJump.fallSpeedLimit = globalParameter_->GetValue<float>(groupName_, "FallSpeedLimit");
    attackRotateEaseT         = globalParameter_->GetValue<float>(groupName_, "attackRotateEaseT");
    attackRotate              = globalParameter_->GetValue<float>(groupName_, "attackRotate");
    attackFloatEaseT          = globalParameter_->GetValue<float>(groupName_, "attackFloatEaseT_");
    attackFloatValue          = globalParameter_->GetValue<float>(groupName_, "attackFloatValue_");
    attackRotateAnit          = globalParameter_->GetValue<float>(groupName_, "attackRotateAnit");

    upperParm.BackLashValue    = globalParameter_->GetValue<float>(groupName_, "upperBackLashValue_");
    upperParm.BackLashEaseTime = globalParameter_->GetValue<float>(groupName_, "upperBackLashEaseTime_");
    upperJump.jumpSpeed        = globalParameter_->GetValue<float>(groupName_, "jumpPowerU");

    bountJump.fallSpeedLimit = globalParameter_->GetValue<float>(groupName_, "fallSpeedLimitB");
    bountJump.gravity        = globalParameter_->GetValue<float>(groupName_, "gravityB");
    bountJump.jumpSpeed      = globalParameter_->GetValue<float>(groupName_, "jumpSpeedB");

    upperJump.fallSpeedLimit = globalParameter_->GetValue<float>(groupName_, "fallSpeedLimitU");
    upperJump.gravity        = globalParameter_->GetValue<float>(groupName_, "gravityU");
    upperJump.jumpSpeed      = globalParameter_->GetValue<float>(groupName_, "jumpPowerU");

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
