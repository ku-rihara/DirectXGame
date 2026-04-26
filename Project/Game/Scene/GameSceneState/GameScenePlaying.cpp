#include "GameScenePlaying.h"
#include "Frame/Frame.h"
#include "GameSceneFinish.h"
#include "GameScenePose.h"
#include "ResultObj/GameResultInfo.h"
#include "Scene/GameScene.h"
#include "utility/DitherOcclusion/DitherOcclusion.h"

GameScenePlaying::GameScenePlaying(GameScene* gameScene)
    : BaseGameSceneState("GameScenePlaying", gameScene) {
    Init();
}

void GameScenePlaying::Init() {
}

bool GameScenePlaying::CheckPauseTransition() {
    return pauseController_.IsPauseTriggered();
}

void GameScenePlaying::Update([[maybe_unused]] float timeSpeed) {
    // ポーズ移行チェック
    if (CheckPauseTransition()) {
        pOwner_->ChangeState(std::make_unique<GameScenePose>(pOwner_));
        return;
    }

    auto& obj = pOwner_->GetGameObj();

    // Editor
    obj.attackEffect_->Update();
    obj.field_->Update();
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
    elapsedTimeSec_ += KetaEngine::Frame::DeltaTime();
    GameResultInfo::GetInstance()->RecordCombo(obj.combo_->GetComboCount());
    GameResultInfo::GetInstance()->RecordKillCount(obj.killCounter_->GetKillCount());
    GameResultInfo::GetInstance()->RecordLevel(obj.deathTimer_->GetCurrentLevel());
    GameResultInfo::GetInstance()->RecordSurvivalTime(static_cast<int32_t>(elapsedTimeSec_));
    obj.fireInjectors_->Update();
    obj.gameCamera_->Update();
    obj.backGroundObjectManager_->Update(timeSpeed);
    obj.deathTimer_->Update(timeSpeed);
    obj.killBonusController_->Update(KetaEngine::Frame::DeltaTime());
    obj.killBonusFlyController_->Update(KetaEngine::Frame::DeltaTime());

    obj.enemyManager_->HpBarUpdate(pOwner_->GetViewProjection());
    obj.enemyManager_->SetViewProjection(&pOwner_->GetViewProjection());
    obj.lockOnController_->Update(obj.player_.get(), pOwner_->GetViewProjection());

    // ディザオクルージョン更新
    Vector3 playerPos = obj.player_->GetBaseTransform().GetWorldPos();
    pOwner_->GetDitherOcclusion()->Update(obj.gameCamera_->GetViewProjection(), playerPos);

    obj.comboSupportSpriteUi_->Update();
    obj.nextAttackHintUI_->Update();

    // ゲーム終了判定
    if (obj.deathTimer_->GetIsDeath()) {
        pOwner_->ChangeState(std::make_unique<GameSceneFinish>(pOwner_));
        return;
    }

    // 全敵撃破チェック
    bool allSpawned = obj.enemySpawner_->GetAllGroupsCompleted() &&
                      !obj.continuousEnemySpawner_->IsActive();

    if (allSpawned && obj.enemyManager_->GetIsAllCleared()) {
        pOwner_->ChangeState(std::make_unique<GameSceneFinish>(pOwner_));
        return;
    }

    ViewUpDate();
}

void GameScenePlaying::Debug() {
}