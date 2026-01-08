#include "GameScenePlaying.h"
#include "Frame/Frame.h"
#include "GameSceneFinish.h"
#include "Scene/GameScene.h"

GameScenePlaying::GameScenePlaying(GameScene* gameScene)
    : BaseGameSceneState("GameScenePlaying", gameScene) {
    Init();
}

void GameScenePlaying::Init() {
}

void GameScenePlaying::Update([[maybe_unused]] float timeSpeed) {
    auto& obj = pOwner_->GetGameObj();

    // Editor
    obj.attackEffect_->Update();
    obj.field_->Update();
    obj.sideRopeController_->Update();
    obj.audienceController_->Update();

    // 各クラス更新
    obj.comboScene_->Update();
    obj.player_->Update();
    obj.skyBox_->Update();
    obj.howToOperate_->Update();
    obj.enemySpawner_->Update(timeSpeed);
    obj.continuousEnemySpawner_->Update(timeSpeed);
    obj.enemyManager_->Update();
    obj.combo_->Update();
    obj.fireInjectors_->Update();
    obj.gameCamera_->Update();
    obj.comboLevelObjHolder_->Update(timeSpeed);
    obj.deathTimer_->Update(timeSpeed);

    obj.enemyManager_->HpBarUpdate(pOwner_->GetViewProjection());
    obj.lockOnController_->Update(obj.player_.get(), pOwner_->GetViewProjection());

    // クリア判定
    if (obj.enemyManager_->GetIsAllCleared() && obj.enemySpawner_->GetAllGroupsCompleted()||obj.player_->GetIsDeathRenditionFinish()) {
        pOwner_->ChangeState(std::make_unique<GameSceneFinish>(pOwner_));
    }
}

void GameScenePlaying::Debug() {
}