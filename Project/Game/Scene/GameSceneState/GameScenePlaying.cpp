#include "GameScenePlaying.h"
#include "Frame/Frame.h"
#include "GameSceneFinish.h"
#include "Scene/GameScene.h"
#include "utility/DitherOcclusion/DitherOcclusion.h"

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
   /* obj.sideRopeController_->Update();*/
    obj.audienceController_->Update();

    // 各クラス更新
    // UI更新
    obj.killCounter_->Update();
    obj.comboAsistController_->Update();
    obj.unlockNotifier_->Update(KetaEngine::Frame::DeltaTime(), &pOwner_->GetViewProjection(), obj.player_.get());
    obj.comboDirector_->Update();
    obj.player_->Update();
    obj.skyBox_->Update();
    obj.operateUI_->Update();
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

    // ディザオクルージョン更新
    Vector3 playerPos = obj.player_->GetBaseTransform().GetWorldPos();
    pOwner_->GetDitherOcclusion()->Update(obj.gameCamera_->GetViewProjection(), playerPos);

    obj.comboSupportSpriteUi_->Update();

    // クリア判定
    if (obj.enemyManager_->GetIsAllCleared() && obj.enemySpawner_->GetAllGroupsCompleted()||obj.player_->GetIsDeathRenditionFinish()) {
        pOwner_->ChangeState(std::make_unique<GameSceneFinish>(pOwner_));
    }
}

void GameScenePlaying::Debug() {
}