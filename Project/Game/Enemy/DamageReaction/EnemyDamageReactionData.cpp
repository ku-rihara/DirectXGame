#include "EnemyDamageReactionData.h"
#include "Function/GetFile.h"
#include <algorithm>
#include <filesystem>
#include <imgui.h>

void EnemyDamageReactionData::Init(const std::string& reactionName) {
    // グローバルパラメータ
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    // グループ名設定
    groupName_ = reactionName;

    // 新規登録
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    reactionParam_.reactionState = static_cast<ReactionState>(reactionParam_.intReactionState);

    // 演出データを生成
    rendition_ = std::make_unique<EnemyDamageRenditionData>();
    rendition_->Init(groupName_);
}

void EnemyDamageReactionData::LoadData() {
    globalParameter_->LoadFile(groupName_, folderPath_);
    // 演出データをロード
    LoadRendition();

    globalParameter_->SyncParamForGroup(groupName_);
}

void EnemyDamageReactionData::SaveData() {
    globalParameter_->SaveFile(groupName_, folderPath_);

    // 演出データを保存
    SaveRendition();
}

///==========================================================
/// 演出データ管理
///==========================================================
void EnemyDamageReactionData::InitRendition() {
    if (rendition_) {
        rendition_->Init(groupName_);
    }
}

void EnemyDamageReactionData::SaveRendition() {
    if (rendition_) {
        rendition_->SaveData();
    }
}

void EnemyDamageReactionData::LoadRendition() {
    // reactionName専用のフォルダパスを作成
    std::string folderPath    = renditionFolderPath_ + groupName_ + "/";
    std::string renditionFile = groupName_ + "_Rendition.json";
    std::string fullPath      = folderPath + renditionFile;

    if (std::filesystem::exists(fullPath)) {
        // 演出データが存在する場合、ロード
        if (rendition_) {
            rendition_->LoadData();
        }
    }
}

///==========================================================
/// バインド
///==========================================================
void EnemyDamageReactionData::RegisterParams() {
    globalParameter_->Regist(groupName_, "TriggerAttackName", &reactionParam_.triggerAttackName);
    globalParameter_->Regist(groupName_, "ReactionState", &reactionParam_.intReactionState);
    globalParameter_->Regist(groupName_, "damageCollingTime", &reactionParam_.damageCollingTime);
    globalParameter_->Regist(groupName_, "isPriorityReaction", &reactionParam_.isPriorityReaction);

    // NormalParam
    globalParameter_->Regist(groupName_, "Normal_KnockBackTime", &reactionParam_.normalParam.knockBackTime);
    globalParameter_->Regist(groupName_, "Normal_KnockBackDamping", &reactionParam_.normalParam.knockBackDamping);

    // BoundParam (共通)
    globalParameter_->Regist(groupName_, "Bound_BoundNum", &reactionParam_.boundParam.boundNum);
    globalParameter_->Regist(groupName_, "Bound_BounceDamping", &reactionParam_.boundParam.bounceDamping);
    globalParameter_->Regist(groupName_, "Bound_InitialBounceRate", &reactionParam_.boundParam.initialBounceRate);

    // SlammedParam
    globalParameter_->Regist(groupName_, "Slammed_Gravity", &reactionParam_.slammedParam.gravity);
    globalParameter_->Regist(groupName_, "Slammed_ThrustRotateSpeed", &reactionParam_.slammedParam.rotateSpeed);

    // TakeUpperParam
    globalParameter_->Regist(groupName_, "TakeUpper_FloatingTime", &reactionParam_.takeUpperParam.floatingTime);
    globalParameter_->Regist(groupName_, "TakeUpper_FallSpeedLimit", &reactionParam_.takeUpperParam.fallSpeedLimit);
    globalParameter_->Regist(groupName_, "TakeUpper_Gravity", &reactionParam_.takeUpperParam.gravity);
    globalParameter_->Regist(groupName_, "TakeUpper_ThrustRotateSpeed", &reactionParam_.takeUpperParam.rotateSpeed);
}

///==========================================================
/// パラメータ調整
///==========================================================
void EnemyDamageReactionData::AdjustParam() {
#ifdef _DEBUG

    ImGui::PushID(groupName_.c_str());

    // Trigger Attack
    ImGui::SeparatorText("Trigger Attack");
    SelectTriggerAttack();

    ImGui::Separator();

    // Reaction State
    ImGui::SeparatorText("Simple Param");
    ImGui::Checkbox("isPriorityReaction", &reactionParam_.isPriorityReaction);
    ImGui::DragFloat("damageCollingTime", &reactionParam_.damageCollingTime, 0.01f);

    ImGui::SeparatorText("Reaction State");
    const char* stateNames[] = {"Normal", "Slammed", "TakeUpper"};
    ImGui::Combo("Reaction Type", &reactionParam_.intReactionState, stateNames, IM_ARRAYSIZE(stateNames));

    // 状態に応じたパラメータ表示
    reactionParam_.reactionState = static_cast<ReactionState>(reactionParam_.intReactionState);

    ImGui::SeparatorText("Normal Parameters");
    ImGui::DragFloat("Knock Back Time", &reactionParam_.normalParam.knockBackTime, 0.01f, 0.0f, 10.0f);
    ImGui::DragFloat("Knock Back Damping", &reactionParam_.normalParam.knockBackDamping, 0.1f);

    // 各状態固有のパラメータ
    if (reactionParam_.reactionState == ReactionState::Slammed) {
        ImGui::SeparatorText("Slammed Parameters");
        ImGui::DragFloat("Gravity", &reactionParam_.slammedParam.gravity, 0.1f, 0.0f);
        ImGui::DragFloat("Rotate Speed", &reactionParam_.slammedParam.rotateSpeed, 0.01f);
    } else if (reactionParam_.reactionState == ReactionState::TakeUpper) {
        ImGui::SeparatorText("Take Upper Parameters");
        ImGui::DragFloat("Floating Time", &reactionParam_.takeUpperParam.floatingTime, 0.01f);
        ImGui::DragFloat("Fall Speed Limit", &reactionParam_.takeUpperParam.fallSpeedLimit, 0.1f);
        ImGui::DragFloat("Gravity", &reactionParam_.takeUpperParam.gravity, 0.1f);
        ImGui::DragFloat("Rotate Speed", &reactionParam_.takeUpperParam.rotateSpeed, 0.01f);
    }

    // SlammedまたはTakeUpperの時、バウンドパラメータを表示
    if (reactionParam_.reactionState == ReactionState::Slammed || reactionParam_.reactionState == ReactionState::TakeUpper) {

        ImGui::SeparatorText("Bound Parameters");
        ImGui::InputInt("Bound Number", &reactionParam_.boundParam.boundNum);
        ImGui::DragFloat("Bounce Damping", &reactionParam_.boundParam.bounceDamping, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Initial Bounce Rate", &reactionParam_.boundParam.initialBounceRate, 0.01f, 0.0f, 2.0f);
    }

    ImGui::Separator();

    // 演出の調整
    if (rendition_) {
        rendition_->AdjustParam();
    }

    ImGui::PopID();

#endif // _DEBUG
}

void EnemyDamageReactionData::SelectTriggerAttack() {
    fileSelector_.SelectFile(
        "Trigger Attack Type",
        "Resources/GlobalParameter/AttackCreator",
        reactionParam_.triggerAttackName,
        groupName_,
        true);
}