#include "ComboScene.h"

#include "BackGroundObject/ComboLeve/ComboLevelObjHolder.h"
#include "Combo/Combo.h"
#include "Player/Player.h"

void ComboScene::Init() {
    // 初期化処理
    state_ = State::CHECK;
}

void ComboScene::Update() {

    // 関数ポインタ呼び出し
    (this->*spFuncTable_[static_cast<size_t>(state_)])();

    // リセット
    if (pCombo_->GetIsReset()) {
        state_ = State::RESET;
    }
}

void ComboScene::CheckIsLevelUp() {

    // 現在レベルを取得
    int32_t currentLevel = pCombo_->GetCurrentLevel();

    if (pCombo_->GetComboCount() < pCombo_->GetLevelUPNum(currentLevel)) {
        return; // レベルアップ条件を満たしていない
    }

    // レベルアップする
    state_ = State::LEVELUP;
}

void ComboScene::LevelUp() {

    pCombo_->LevelUp();

    // レベル対象のオブジェクトをスポーン
    if (pCombo_->GetCurrentLevel() <= static_cast<int32_t>(ComboLevelObjType::COUNT)) {
        comboLevelObjHolder_->SetEffectMode(static_cast<ComboLevelObjType>(pCombo_->GetCurrentLevel() - levelOffset_), ObjEffectMode::SPAWN);
    }

    // チェックに戻す
    state_ = State::CHECK;
}

void ComboScene::LevelReset() {
    // 背景オブジェクトClose
    comboLevelObjHolder_->CloseForComboLevel(pCombo_->GetCurrentLevel());
    // コンボリセット
    pCombo_->Reset();
    // チェックに戻す
    state_ = State::CHECK;
}

///--------------------------------------------------------------------------------
/// class Set
///--------------------------------------------------------------------------------
void ComboScene::SetPlayer(Player* player) {
    pPlayer_ = player;
}

void ComboScene::SetCombo(Combo* combo) {
    pCombo_ = combo;
}

void ComboScene::SetComboLevelObjHolder(ComboLevelObjHolder* comboLevelObjHolder) {
    comboLevelObjHolder_ = comboLevelObjHolder;
}

void ComboScene::SetAudienceController(AudienceController* audienceController) {
    audienceController_ = audienceController;
}

/// --------------------------------------------------------------------------------
// メンバ関数のポインタ配列
/// --------------------------------------------------------------------------------
void (ComboScene::* ComboScene::spFuncTable_[])(){
    &ComboScene::CheckIsLevelUp,
    &ComboScene::LevelUp,
    &ComboScene::LevelReset,
};
