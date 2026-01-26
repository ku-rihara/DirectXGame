#include "PlayerComboAttackData.h"
// Easing
#include "Easing/EasingCreator/EasingParameterData.h"
// input
#include "input/Input.h"
#include "input/InputData.h"
// imGui
#include <imgui.h>

void PlayerComboAttackData::Init(const std::string& attackName) {
    // グローバルパラメータ
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();

    // グループ作成、パラメータ登録、同期
    groupName_ = attackName;
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    // conditionをIntから適応
    attackParam_.triggerParam.condition = static_cast<TriggerCondition>(triggerConditionInt_);

    // タイムラインの初期化
    timeLine_.Init(this);
}

void PlayerComboAttackData::LoadData() {
    globalParameter_->LoadFile(groupName_, folderPath_);
    globalParameter_->SyncParamForGroup(groupName_);
}

void PlayerComboAttackData::SaveData() {
    globalParameter_->SaveFile(groupName_, folderPath_);
}

///==========================================================
/// バインド
///==========================================================
void PlayerComboAttackData::RegisterParams() {
    // simpleParam
    globalParameter_->Regist(groupName_, "power", &attackParam_.power);
    globalParameter_->Regist(groupName_, "KnockBackPower", &attackParam_.knockBackPower);
    globalParameter_->Regist(groupName_, "blowYPower", &attackParam_.blowYPower);
    globalParameter_->Regist(groupName_, "isMotionOnly", &attackParam_.isMotionOnly);

    // CollisionParam
    globalParameter_->Regist(groupName_, "collisionSize", &attackParam_.collisionParam.size);
    globalParameter_->Regist(groupName_, "collisionOffsetPos", &attackParam_.collisionParam.offsetPos);
    globalParameter_->Regist(groupName_, "collisionStartTime", &attackParam_.collisionParam.startTime); // 追加
    globalParameter_->Regist(groupName_, "adaptTime", &attackParam_.collisionParam.adaptTime);
    globalParameter_->Regist(groupName_, "loopWaitTime", &attackParam_.collisionParam.loopWaitTime);
    globalParameter_->Regist(groupName_, "loopNum", &attackParam_.collisionParam.loopNum);
    globalParameter_->Regist(groupName_, "isAlwaysFollowing", &attackParam_.collisionParam.isAlwaysFollowing);

    // MoveParam
    globalParameter_->Regist(groupName_, "moveValue", &attackParam_.moveParam.value);
    globalParameter_->Regist(groupName_, "moveEaseType", &attackParam_.moveParam.easeType);
    globalParameter_->Regist(groupName_, "moveStartTime", &attackParam_.moveParam.startTime);
    globalParameter_->Regist(groupName_, "moveEaseTime", &attackParam_.moveParam.easeTime);
    globalParameter_->Regist(groupName_, "isAbleInputMoving", &attackParam_.moveParam.isAbleInputMoving);
    globalParameter_->Regist(groupName_, "isPositionYSelect", &attackParam_.moveParam.isPositionYSelect);
    globalParameter_->Regist(groupName_, "finishTimeOffset", &attackParam_.moveParam.finishTimeOffset);

    // TriggerParam
    globalParameter_->Regist(groupName_, "gamePadBottom", &attackParam_.triggerParam.gamePadBottom);
    globalParameter_->Regist(groupName_, "keyBordBottom", &attackParam_.triggerParam.keyBordBottom);
    globalParameter_->Regist(groupName_, "Condition", &triggerConditionInt_);
    globalParameter_->Regist(groupName_, "IsFirstAttack", &attackParam_.triggerParam.isFirstAttack);
    globalParameter_->Regist(groupName_, "RequireHit", &attackParam_.triggerParam.requireHit);
    globalParameter_->Regist(groupName_, "isAutoAdvance", &attackParam_.timingParam.isAutoAdvance);

    // TimingParam
    globalParameter_->Regist(groupName_, "isCancel", &attackParam_.timingParam.isCancel);
    globalParameter_->Regist(groupName_, "cancelFrame", &attackParam_.timingParam.cancelTime);
    globalParameter_->Regist(groupName_, "precedeInputFrame", &attackParam_.timingParam.precedeInputTime);
    globalParameter_->Regist(groupName_, "finishWaitTime", &attackParam_.timingParam.finishWaitTime);

    // FallParam
    globalParameter_->Regist(groupName_, "enableFall", &attackParam_.fallParam.enableFall);

    // nextAttack
    globalParameter_->Regist(groupName_, "NextAttackType", &attackParam_.nextAttackType);

    // 演出のパラメータバインド
    renditionData_.RegisterParams(globalParameter_, groupName_);
}

///==========================================================
/// パラメータ調整
///==========================================================
void PlayerComboAttackData::AdjustParam() {
#ifdef _DEBUG

    ImGui::PushID(groupName_.c_str());
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), (groupName_ + " Editing").c_str());

    // 　タイムライン描画
    timeLine_.Draw();

    ImGui::PopID();

#endif // _DEBUG
}

void PlayerComboAttackData::DrawCollisionParamUI() {
    auto& collisionParam = attackParam_.collisionParam;

    ImGui::SeparatorText("コライダーパラメータ");

    ImGui::DragFloat3("サイズ", &collisionParam.size.x, 0.01f);
    ImGui::DragFloat3("オフセット位置", &collisionParam.offsetPos.x, 0.01f);
    ImGui::InputInt("ループ回数", &collisionParam.loopNum);

    if (collisionParam.loopNum > 0) {
        ImGui::DragFloat("ループ待機時間", &collisionParam.loopWaitTime, 0.01f);
    }

    ImGui::Checkbox("プレイヤーに追従する", &collisionParam.isAlwaysFollowing);

    ImGui::Separator();
    ImGui::DragFloat("攻撃力", &attackParam_.power, 0.01f);
    ImGui::DragFloat("正面のノックバック力", &attackParam_.knockBackPower, 0.01f);
    ImGui::DragFloat("Y方向のノックバック力", &attackParam_.blowYPower, 0.01f);
}

void PlayerComboAttackData::DrawMoveParamUI() {
    auto& moveParam = attackParam_.moveParam;

    ImGui::SeparatorText("移動パラメータ");

    ImGui::Checkbox("攻撃中入力による移動ができる", &moveParam.isAbleInputMoving);
    ImGui::DragFloat("開始時間", &moveParam.startTime, 0.01f);
    ImGui::Checkbox("Yの位置を直接指定する", &moveParam.isPositionYSelect);
    ImGui::DragFloat3("移動量", &moveParam.value.x, 0.01f);
    ImGui::DragFloat("終了タイムオフセット", &moveParam.finishTimeOffset, 0.01f);

    // Easing Type
    ImGuiEasingTypeSelector("イージング", moveParam.easeType);
}

void PlayerComboAttackData::DrawTriggerParamUI() {
    auto& triggerParam = attackParam_.triggerParam;

    ImGui::SeparatorText("攻撃発動パラメータ");

    ImGui::Checkbox("最初の攻撃", &triggerParam.isFirstAttack);
    ImGui::Checkbox("前の攻撃がヒットした時が発動条件", &triggerParam.requireHit);

    ImGuiKeyboardKeySelector("キーボード:ボタン", triggerParam.keyBordBottom);
    ImGuiGamepadButtonSelector("パッド:ボタン", triggerParam.gamePadBottom);

    // 発動条件
    const char* conditionItems[] = {"地面", "空中", "両方"};
    triggerConditionInt_         = static_cast<int>(triggerParam.condition);
    if (ImGui::Combo("発動できる状況", &triggerConditionInt_,
            conditionItems, IM_ARRAYSIZE(conditionItems))) {
        triggerParam.condition = static_cast<TriggerCondition>(triggerConditionInt_);
    }
}

void PlayerComboAttackData::DrawFlagsParamUI() {
    ImGui::SeparatorText("その他フラグ設定");

    ImGui::Checkbox("モーションのみ有効", &attackParam_.isMotionOnly);
    ImGui::Checkbox("自動で次の攻撃に進む", &attackParam_.timingParam.isAutoAdvance);
    ImGui::Checkbox("攻撃終了時に落ちる", &attackParam_.fallParam.enableFall);
}

void PlayerComboAttackData::SelectNextAttack() {
    fileSelector_.SelectFile(
        "次の攻撃名",
        "Resources/GlobalParameter/AttackCreator",
        attackParam_.nextAttackType,
        groupName_,
        true);
}

void PlayerComboAttackData::DrawSaveLoadUI() {

    globalParameter_->ParamSaveForImGui(groupName_, folderPath_);
    globalParameter_->ParamLoadForImGui(groupName_, folderPath_);
}

bool PlayerComboAttackData::IsReserveNextAttack(float currentTime, const TriggerParam& nextAtkTrigger, bool hasHitEnemy) {

    // 先行入力受付
    if (currentTime < attackParam_.timingParam.precedeInputTime && !IsWaitFinish(currentTime)) {
        return false;
    }

    // ヒット条件チェック
    if (nextAtkTrigger.requireHit && !hasHitEnemy) {
        return false;
    }

    // キーボード入力チェック
    if (KetaEngine::Input::GetInstance()->TriggerKey(FromDIKCode(nextAtkTrigger.keyBordBottom))) {
        return true;
    }

    // ゲームパッド入力チェック
    if (KetaEngine::Input::IsTriggerPad(0, FromXInputButtonFlag(nextAtkTrigger.gamePadBottom))) {
        return true;
    }

    return false;
}

bool PlayerComboAttackData::IsCancelAttack(float currentTime, const TriggerParam& nextAtkTrigger, bool hasHitEnemy) {

    if (!attackParam_.timingParam.isCancel) {
        return false;
    }

    if (currentTime < attackParam_.timingParam.cancelTime) {
        return false;
    }

    // ヒット条件チェック
    if (nextAtkTrigger.requireHit && !hasHitEnemy) {
        return false;
    }

    // キーボード入力チェック
    if (KetaEngine::Input::GetInstance()->TriggerKey(FromDIKCode(nextAtkTrigger.keyBordBottom))) {
        return true;
    }

    // ゲームパッド入力チェック
    if (KetaEngine::Input::IsTriggerPad(0, FromXInputButtonFlag(nextAtkTrigger.gamePadBottom))) {
        return true;
    }

    return false;
}

bool PlayerComboAttackData::IsWaitFinish(float currentTime) {
    if (currentTime >= attackParam_.timingParam.finishWaitTime) {
        return true;
    } else {
        return false;
    }
}

 void PlayerComboAttackData::SetPlayer(Player* player) {
     pPlayer_ = player;
 }