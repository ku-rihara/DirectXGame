#include "ComboDirector.h"
#include "Scene/GameObj.h"

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

    // 最大レベルならレベルアップチェックを行わない
    if (currentLevel >= kComboLevel) {
        return;
    }

    if (pCombo_->GetComboCount() < pCombo_->GetLevelUPNum(currentLevel)) {
        return; // レベルアップ条件を満たしていない
    }

    // レベルアップする
    state_ = State::LEVELUP;
}

void ComboDirector::LevelUp() {

    pCombo_->LevelUp();

    // 累積的にすべてのレベルのオブジェクトと観客をスポーン
    for (int32_t i = 1; i <= pCombo_->GetCurrentLevel(); ++i) {
        comboLevelObjHolder_->SetEffectMode(i, ObjEffectMode::SPAWN);
        audienceController_->AppearAudienceByLevel(i);
    }

    // チェックに戻す
    state_ = State::CHECK;
}

void ComboDirector::LevelReset() {
    // 現在のレベルまですべての背景オブジェクトと観客を閉じる
    int32_t currentLevel = pCombo_->GetCurrentLevel();
    comboLevelObjHolder_->CloseForComboLevel(currentLevel);
    audienceController_->DisAppearAudience(currentLevel);

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

void ComboDirector::SetComboAndStressGauge(Combo* combo, StressGauge* StressGauge) {
    pCombo_ = combo;
    pCombo_->SetStressGauge(StressGauge);
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

void ComboDirector::Connect(GameObj* go) {
    SetPlayer(go->player_.get());
    SetComboAndStressGauge(go->combo_.get(), go->StressGauge_.get());
    SetComboLevelObjHolder(go->backGroundObjectManager_->GetComboLevelObjHolder());
    SetAudienceController(go->audienceController_.get());
}