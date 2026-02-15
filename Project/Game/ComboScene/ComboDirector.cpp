#include "ComboDirector.h"

#include "Audience/AudienceController.h"
#include "BackGroundObject/ComboLevel/ComboLevelObjHolder.h"
#include "Combo/Combo.h"
#include "Player/Player.h"

void ComboDirector::Init() {
    // 初期化処理
    state_ = State::CHECK;
}

void ComboDirector::Update() {

    // 関数ポインタ呼び出し
    (this->*spFuncTable_[static_cast<size_t>(state_)])();

    // リセット
    if (pCombo_->GetIsReset()) {
        state_ = State::RESET;
    }
}

void ComboDirector::CheckIsLevelUp() {

    // 現在レベルを取得
    int32_t currentLevel = pCombo_->GetCurrentLevel();

    if (pCombo_->GetComboCount() < pCombo_->GetLevelUPNum(currentLevel)) {
        return; // レベルアップ条件を満たしていない
    }

    // レベルアップする
    state_ = State::LEVELUP;
}

void ComboDirector::LevelUp() {

    pCombo_->LevelUp();

    // レベル対象のオブジェクト、観客をスポーン
    if (pCombo_->GetCurrentLevel() <= static_cast<int32_t>(ComboLevelObjType::COUNT)) {

        int32_t comboLevel = GetComboLevelZeroStart();

        comboLevelObjHolder_->SetEffectMode(comboLevel, ObjEffectMode::SPAWN);
        audienceController_->AppearAudienceByLevel(comboLevel);
    }

    // チェックに戻す
    state_ = State::CHECK;
}

void ComboDirector::LevelReset() {
    int32_t comboLevel = GetComboLevelZeroStart();
    // 背景オブジェクトClose
    comboLevelObjHolder_->CloseForComboLevel(comboLevel);
    audienceController_->DisAppearAudience(comboLevel);
    // コンボリセット
    pCombo_->Reset();
    // チェックに戻す
    state_ = State::CHECK;
}

int32_t ComboDirector::GetComboLevelZeroStart() {

    return pCombo_->GetCurrentLevel() - levelOffset_;
}

///--------------------------------------------------------------------------------
/// class Set
///--------------------------------------------------------------------------------
void ComboDirector::SetPlayer(Player* player) {
    pPlayer_ = player;
}

void ComboDirector::SetComboAndDeathTimer(Combo* combo, DeathTimer* deathTimer) {
    pCombo_ = combo;
    pCombo_->SetDeathTimer(deathTimer);
}

void ComboDirector::SetComboLevelObjHolder(ComboLevelObjHolder* comboLevelObjHolder) {
    comboLevelObjHolder_ = comboLevelObjHolder;
}

void ComboDirector::SetAudienceController(AudienceController* audienceController) {
    audienceController_ = audienceController;
}


/// --------------------------------------------------------------------------------
// メンバ関数のポインタ配列
/// --------------------------------------------------------------------------------
void (ComboDirector::* ComboDirector::spFuncTable_[])(){
    &ComboDirector::CheckIsLevelUp,
    &ComboDirector::LevelUp,
    &ComboDirector::LevelReset,
};
