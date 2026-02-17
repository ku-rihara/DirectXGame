#include "PlayerComboAttackData.h"
#include "PlayerComboAttackController.h"
// Easing
#include "Easing/EasingCreator/EasingParameterData.h"
// Enemy
#include "Enemy/EnemyManager.h"
// Player
#include "Player/Player.h"
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

    // 解放フラグ初期化
    attackParam_.isUnlocked = (attackParam_.ableDefeatLevel == 0);

    // コンボ分岐データを初期化
    InitComboBranches();

    // タイムラインの初期化
    timeLine_.Init(this);
}

void PlayerComboAttackData::LoadData() {
    // JSONファイルからロードして同期
    globalParameter_->LoadFile(groupName_, folderPath_);
    globalParameter_->SyncParamForGroup(groupName_);

    // conditionをIntから適応
    attackParam_.triggerParam.condition = static_cast<TriggerCondition>(triggerConditionInt_);

    // 解放フラグ初期化
    attackParam_.isUnlocked = (attackParam_.ableDefeatLevel == 0);
}

void PlayerComboAttackData::SaveData() {
    // branchCount_を更新
    branchCount_ = static_cast<int32_t>(comboBranches_.size());

    globalParameter_->SaveFile(groupName_, folderPath_);
}

///==========================================================
/// コンボ分岐データの初期化
///==========================================================
void PlayerComboAttackData::InitComboBranches() {

    comboBranches_.clear();

    if (branchCount_ == 0) {
        return;
    }

    comboBranches_.reserve(branchCount_);

    // 各分岐のComboBranchParameterを初期化
    for (int32_t i = 0; i < branchCount_; ++i) {
        auto branch = std::make_unique<ComboBranchParameter>();
        branch->Init(groupName_, i);
        comboBranches_.push_back(std::move(branch));
    }
}

///==========================================================
/// TriggerConditionのチェック
///==========================================================
bool PlayerComboAttackData::CheckTriggerCondition(TriggerCondition condition) const {
    switch (condition) {
    case TriggerCondition::GROUND:
    case TriggerCondition::AIR:
    case TriggerCondition::BOTH:

        return true;

    case TriggerCondition::DASH:
        if (pPlayer_) {
            return pPlayer_->IsDashing();
        }
        return false;

    case TriggerCondition::JUSTACTION:
        if (pEnemyManager_) {
            return pEnemyManager_->IsAnyEnemyInAnticipation();
        }
        return false;

    default:
        return true;
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

    // 解放レベル
    globalParameter_->Regist(groupName_, "ableDefeatLevel", &attackParam_.ableDefeatLevel);

    // コンボ分岐数
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

    // セーブ・ロードボタン
    DrawSaveLoadUI();
    ImGui::Separator();

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

    // 最初の攻撃かどうかを表示
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
        const char* conditionItems[] = {"地上", "空中", "両方", "ダッシュ", "ジャストアクション"};
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

    ImGui::SeparatorText("解放設定");
    ImGui::InputInt("解放に必要なレベル", &attackParam_.ableDefeatLevel);
    if (attackParam_.ableDefeatLevel < 0) {
        attackParam_.ableDefeatLevel = 0;
    }
}

void PlayerComboAttackData::DrawComboBranchesUI() {
    ImGui::SeparatorText("コンボ分岐設定");

    // 分岐がない場合
    if (comboBranches_.empty()) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "コンボ分岐がありません");
    }

    // 削除対象のインデックス
    int32_t removeIndex = -1;

    // 各分岐のUIを描画
    size_t branchSize = comboBranches_.size();
    for (size_t i = 0; i < branchSize; ++i) {
        auto& branch = comboBranches_[i];
        if (!branch) {
            continue; // nullチェック
        }

        ImGui::PushID(static_cast<int>(i));

        // 分岐のパラメータUIを描画
        if (branch->DrawParameterUI("Resources/GlobalParameter/AttackCreator", groupName_)) {
            removeIndex = static_cast<int32_t>(i);
        }

        ImGui::PopID();
    }

    // 削除処理
    if (removeIndex >= 0 && removeIndex < static_cast<int32_t>(comboBranches_.size())) {
        // 削除前にbranchCount_を更新
        branchCount_ = static_cast<int32_t>(comboBranches_.size()) - 1;

        // イテレータを使って安全に削除
        auto it = comboBranches_.begin() + removeIndex;
        comboBranches_.erase(it);

        // タイムラインの分岐トラックを再構築
        RebuildBranchTracks();
    }

    // 追加ボタン
    ImGui::Separator();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.5f, 0.1f, 1.0f));
    if (ImGui::Button("+ コンボ分岐を追加")) {
        int32_t newIndex = static_cast<int32_t>(comboBranches_.size());
        auto newBranch   = std::make_unique<ComboBranchParameter>();
        newBranch->Init(groupName_, newIndex);
        comboBranches_.push_back(std::move(newBranch));
        // branchCount_を更新
        branchCount_ = static_cast<int32_t>(comboBranches_.size());
        // タイムラインの分岐トラックを再構築
        RebuildBranchTracks();
    }
    ImGui::PopStyleColor(3);

    // トラック名更新ボタン
    if (!comboBranches_.empty()) {
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

bool PlayerComboAttackData::IsReserveNextAttack(float currentTime, const ComboBranchParameter& branch, bool hasHitEnemy) {

    // 先行入力受付時間チェック
    if (currentTime < branch.GetPrecedeInputTime() && !IsWaitFinish(currentTime)) {
        return false;
    }

    // ヒット条件チェック
    if (branch.GetRequireHit() && !hasHitEnemy) {
        return false;
    }

    // 発動条件は最初の攻撃開始時のみチェックされるため、コンボ継続時はチェックしない

    // キーボード入力チェック
    if (KetaEngine::Input::GetInstance()->TriggerKey(FromDIKCode(branch.GetKeyboardButton()))) {
        return true;
    }

    // ゲームパッド入力チェック
    if (KetaEngine::Input::IsTriggerPad(0, FromXInputButtonFlag(branch.GetGamepadButton()))) {
        return true;
    }

    return false;
}

bool PlayerComboAttackData::IsCancelAttack(float currentTime, const ComboBranchParameter& branch, bool hasHitEnemy) {

    // キャンセル時間チェック
    if (currentTime < branch.GetCancelTime()) {
        return false;
    }

    // ヒット条件チェック
    if (branch.GetRequireHit() && !hasHitEnemy) {
        return false;
    }

    // 発動条件は最初の攻撃開始時のみチェックされるため、コンボ継続時はチェックしない

    // キーボード入力チェック
    if (KetaEngine::Input::GetInstance()->TriggerKey(FromDIKCode(branch.GetKeyboardButton()))) {
        return true;
    }

    // ゲームパッド入力チェック
    if (KetaEngine::Input::IsTriggerPad(0, FromXInputButtonFlag(branch.GetGamepadButton()))) {
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