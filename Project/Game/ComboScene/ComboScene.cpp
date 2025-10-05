#include "ComboScene.h"

#include "BackGroundObject/ComboLeve/StadiumLightEffect.h"
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
    const int32_t& currentLevel = pCombo_->GetCurrentLevel();

    if (pCombo_->GetComboCount() < pCombo_->GetLevelUPNum(currentLevel)) {
        return; // レベルアップ条件を満たしていない
    }

    // レベルアップする
    state_ = State::LEVELUP;
}

void ComboScene::LevelUp() {

    pCombo_->LevelUp();

    // レベルアップにまつわる演出を呼び出す
    switch (pCombo_->GetCurrentLevel()) {
    case 1:
        stadiumLightEffect_->SetEffectMode(ObjEffectMode::SPAWN);
        break;
            break;
    default:
        break;
    }

    // チェックに戻す
    state_ = State::CHECK;
}

void ComboScene::LevelReset() {
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

void ComboScene::SetStadiumLightEffect(StadiumLightEffect* stadiumLightEffect) {
    stadiumLightEffect_ = stadiumLightEffect;
}

/// --------------------------------------------------------------------------------
// メンバ関数のポインタ配列
/// --------------------------------------------------------------------------------
void (ComboScene::* ComboScene::spFuncTable_[])(){
    &ComboScene::CheckIsLevelUp,
    &ComboScene::LevelUp,
    &ComboScene::LevelReset,
};
