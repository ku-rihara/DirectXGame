#include "PlayerComboAttackData.h"
#include "PlayerComboAttackController.h"
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

    // コンボ分岐データをロード（Registを使わない）
    LoadComboBranches();

    // タイムラインの初期化
    timeLine_.Init(this);
}

void PlayerComboAttackData::LoadData() {
    globalParameter_->LoadFile(groupName_, folderPath_);
    globalParameter_->SyncParamForGroup(groupName_);

    // conditionをIntから適応
    attackParam_.triggerParam.condition = static_cast<TriggerCondition>(triggerConditionInt_);

    // コンボ分岐データをロード（Registを使わない）
    LoadComboBranches();
}

void PlayerComboAttackData::SaveData() {
    // コンボ分岐データをセーブ（Registを使わない）
    SaveComboBranches();

    globalParameter_->SaveFile(groupName_, folderPath_);
}

///==========================================================
/// コンボ分岐データのロード（GetValueを使用）
///==========================================================
void PlayerComboAttackData::LoadComboBranches() {
    // branchCount_はRegistでバインド済みなので、そのまま使用
    // 分岐配列をリサイズ
    attackParam_.comboBranches.resize(branchCount_);
    branchFileSelectors_.resize(branchCount_);

    // 各分岐のデータを読み込み
    for (int32_t i = 0; i < branchCount_; ++i) {
        std::string prefix = "Branch_" + std::to_string(i) + "_";
        auto& branch = attackParam_.comboBranches[i];

        branch.nextAttackName = globalParameter_->GetValue<std::string>(groupName_, prefix + "nextAttackName");
        branch.keyboardButton = globalParameter_->GetValue<int32_t>(groupName_, prefix + "keyboardButton");
        branch.gamepadButton = globalParameter_->GetValue<int32_t>(groupName_, prefix + "gamepadButton");
        branch.cancelTime = globalParameter_->GetValue<float>(groupName_, prefix + "cancelTime");
        branch.precedeInputTime = globalParameter_->GetValue<float>(groupName_, prefix + "precedeInputTime");
        branch.requireHit = globalParameter_->GetValue<bool>(groupName_, prefix + "requireHit");
    }
}

///==========================================================
/// コンボ分岐データのセーブ（SetValueを使用）
///==========================================================
void PlayerComboAttackData::SaveComboBranches() {
    // 分岐数を更新
    branchCount_ = static_cast<int32_t>(attackParam_.comboBranches.size());
    globalParameter_->SetValue(groupName_, "ComboBranchCount", branchCount_);

    // 各分岐のデータを書き込み
    for (int32_t i = 0; i < branchCount_; ++i) {
        std::string prefix = "Branch_" + std::to_string(i) + "_";
        const auto& branch = attackParam_.comboBranches[i];

        globalParameter_->SetValue(groupName_, prefix + "nextAttackName", branch.nextAttackName);
        globalParameter_->SetValue(groupName_, prefix + "keyboardButton", branch.keyboardButton);
        globalParameter_->SetValue(groupName_, prefix + "gamepadButton", branch.gamepadButton);
        globalParameter_->SetValue(groupName_, prefix + "cancelTime", branch.cancelTime);
        globalParameter_->SetValue(groupName_, prefix + "precedeInputTime", branch.precedeInputTime);
        globalParameter_->SetValue(groupName_, prefix + "requireHit", branch.requireHit);
    }
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
    globalParameter_->Regist(groupName_, "collisionStartTime", &attackParam_.collisionParam.startTime);
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

    // TriggerParam（最初の攻撃用）
    globalParameter_->Regist(groupName_, "gamePadBottom", &attackParam_.triggerParam.gamePadBottom);
    globalParameter_->Regist(groupName_, "keyBordBottom", &attackParam_.triggerParam.keyBordBottom);
    globalParameter_->Regist(groupName_, "Condition", &triggerConditionInt_);

    // TimingParam
    globalParameter_->Regist(groupName_, "finishWaitTime", &attackParam_.timingParam.finishWaitTime);
    globalParameter_->Regist(groupName_, "isAutoAdvance", &attackParam_.timingParam.isAutoAdvance);

    // FallParam
    globalParameter_->Regist(groupName_, "enableFall", &attackParam_.fallParam.enableFall);

    // コンボ分岐数（読み込み用にRegistしておく）
    globalParameter_->Regist(groupName_, "ComboBranchCount", &branchCount_);

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

    // タイムライン描画
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

void PlayerComboAttackData::DrawTriggerParamUI(bool isFirstAttack) {
    auto& triggerParam = attackParam_.triggerParam;

    ImGui::SeparatorText("攻撃発動パラメータ");

    // 最初の攻撃かどうかを表示（自動判定）
    if (isFirstAttack) {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "これは最初の攻撃です");
    } else {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "これはコンボ中の攻撃です");
    }

    // 最初の攻撃の場合のみボタンと状況を表示
    if (isFirstAttack) {
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
}

void PlayerComboAttackData::DrawFlagsParamUI() {
    ImGui::SeparatorText("その他フラグ設定");

    ImGui::Checkbox("モーションのみ有効", &attackParam_.isMotionOnly);
    ImGui::Checkbox("自動で次の攻撃に進む", &attackParam_.timingParam.isAutoAdvance);
    ImGui::Checkbox("攻撃終了時に落ちる", &attackParam_.fallParam.enableFall);
}

void PlayerComboAttackData::DrawComboBranchesUI() {
    ImGui::SeparatorText("コンボ分岐設定");

    auto& branches = attackParam_.comboBranches;

    // 分岐がない場合
    if (branches.empty()) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "コンボ分岐がありません");
    }

    // 削除対象のインデックス（-1なら削除しない）
    int32_t removeIndex = -1;

    // 各分岐を表示
    for (size_t i = 0; i < branches.size(); ++i) {
        auto& branch = branches[i];
        ImGui::PushID(static_cast<int>(i));

        // 分岐ヘッダー
        std::string header = "分岐 " + std::to_string(i + 1);
        if (!branch.nextAttackName.empty()) {
            header += " -> " + branch.nextAttackName;
        }

        if (ImGui::CollapsingHeader(header.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Indent();

            // FileSelector が足りなければ追加
            while (branchFileSelectors_.size() <= i) {
                branchFileSelectors_.push_back(KetaEngine::FileSelector());
            }

            // 次の攻撃名選択
            branchFileSelectors_[i].SelectFile(
                "次の攻撃名",
                "Resources/GlobalParameter/AttackCreator",
                branch.nextAttackName,
                groupName_ + "_branch_" + std::to_string(i),
                true);

            // ボタン設定
            ImGui::Separator();
            ImGui::Text("入力ボタン:");
            ImGuiKeyboardKeySelector("キーボード", branch.keyboardButton);
            ImGuiGamepadButtonSelector("ゲームパッド", branch.gamepadButton);

            // タイミング設定
            ImGui::Separator();
            ImGui::Text("タイミング:");
            ImGui::DragFloat("キャンセル開始時間", &branch.cancelTime, 0.01f, 0.0f, 10.0f);
            ImGui::DragFloat("先行入力開始時間", &branch.precedeInputTime, 0.01f, 0.0f, 10.0f);

            // 条件設定
            ImGui::Separator();
            ImGui::Checkbox("ヒット必須", &branch.requireHit);

            // 削除ボタン
            ImGui::Separator();
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.1f, 0.1f, 1.0f));
            if (ImGui::Button("この分岐を削除")) {
                removeIndex = static_cast<int32_t>(i);
            }
            ImGui::PopStyleColor(3);

            ImGui::Unindent();
        }

        ImGui::PopID();
    }

    // 削除処理
    if (removeIndex >= 0 && removeIndex < static_cast<int32_t>(branches.size())) {
        branches.erase(branches.begin() + removeIndex);
        if (removeIndex < static_cast<int32_t>(branchFileSelectors_.size())) {
            branchFileSelectors_.erase(branchFileSelectors_.begin() + removeIndex);
        }
        // タイムラインの分岐トラックを再構築
        RebuildBranchTracks();
    }

    // 追加ボタン
    ImGui::Separator();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.5f, 0.1f, 1.0f));
    if (ImGui::Button("+ コンボ分岐を追加")) {
        ComboBranch newBranch;
        branches.push_back(newBranch);
        branchFileSelectors_.push_back(KetaEngine::FileSelector());
        // タイムラインの分岐トラックを再構築
        RebuildBranchTracks();
    }
    ImGui::PopStyleColor(3);

    // トラック名更新ボタン（ボタン設定変更後に使用）
    if (!branches.empty()) {
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.4f, 0.8f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 1.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.3f, 0.7f, 1.0f));
        if (ImGui::Button("トラック名を更新")) {
            RebuildBranchTracks();
        }
        ImGui::PopStyleColor(3);
    }
}

void PlayerComboAttackData::DrawSaveLoadUI() {

    globalParameter_->ParamSaveForImGui(groupName_, folderPath_);
    globalParameter_->ParamLoadForImGui(groupName_, folderPath_);
}

void PlayerComboAttackData::RebuildBranchTracks() {
    timeLine_.RebuildBranchTracks();
}

bool PlayerComboAttackData::IsReserveNextAttack(float currentTime, const ComboBranch& branch, bool hasHitEnemy) {

    // 先行入力受付時間チェック
    if (currentTime < branch.precedeInputTime && !IsWaitFinish(currentTime)) {
        return false;
    }

    // ヒット条件チェック
    if (branch.requireHit && !hasHitEnemy) {
        return false;
    }

    // キーボード入力チェック
    if (KetaEngine::Input::GetInstance()->TriggerKey(FromDIKCode(branch.keyboardButton))) {
        return true;
    }

    // ゲームパッド入力チェック
    if (KetaEngine::Input::IsTriggerPad(0, FromXInputButtonFlag(branch.gamepadButton))) {
        return true;
    }

    return false;
}

bool PlayerComboAttackData::IsCancelAttack(float currentTime, const ComboBranch& branch, bool hasHitEnemy) {

    // キャンセル時間チェック
    if (currentTime < branch.cancelTime) {
        return false;
    }

    // ヒット条件チェック
    if (branch.requireHit && !hasHitEnemy) {
        return false;
    }

    // キーボード入力チェック
    if (KetaEngine::Input::GetInstance()->TriggerKey(FromDIKCode(branch.keyboardButton))) {
        return true;
    }

    // ゲームパッド入力チェック
    if (KetaEngine::Input::IsTriggerPad(0, FromXInputButtonFlag(branch.gamepadButton))) {
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

KetaEngine::TimelineDrawer* PlayerComboAttackData::GetTimeline() {
    return timeLine_.GetTimeline();
}
