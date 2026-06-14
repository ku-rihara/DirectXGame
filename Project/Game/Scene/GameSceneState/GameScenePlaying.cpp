#include "GameScenePlaying.h"
#include "StressGauge/StressGaugeGauge.h"
#include "Frame/Frame.h"
#include "GameSceneGameOver.h"
#include "GameScenePose.h"
#include "input/Input.h"
#include "PostEffect/PostEffectRenderer.h"
#include "ResultObj/GameResultInfo.h"
#include "Scene/GameScene.h"
#include "utility/DitherOcclusion/DitherOcclusion.h"
#include <cmath>

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

    // Editor更新
    obj.field_->Update();
    obj.audienceController_->Update();

    // UI更新
    obj.killCounter_->Update();
    obj.comboAssistController_->Update();
    obj.unlockNotifier_->Update(KetaEngine::Frame::DeltaTime(), &pOwner_->GetViewProjection(), obj.player_.get());
    obj.comboDirector_->Update();
    obj.operateUI_->Update();

    // プレイヤー・ワールド更新
    obj.player_->Update();
    obj.skyBox_->Update();
    obj.fireInjectors_->Update();
    obj.gameCamera_->Update();
    obj.backGroundObjectManager_->Update(timeSpeed);

    // 敵管理更新
    obj.enemyManager_->GetSpawner()->Update(timeSpeed);
    obj.continuousEnemySpawner_->Update(timeSpeed);
    obj.enemyManager_->Update();

    // コンボ・リザルト記録
    obj.combo_->Update();
    elapsedTimeSec_ += KetaEngine::Frame::DeltaTime();
    GameResultInfo::GetInstance()->RecordCombo(obj.combo_->GetComboCount());
    GameResultInfo::GetInstance()->RecordKillCount(obj.killCounter_->GetKillCount());
    GameResultInfo::GetInstance()->RecordSurvivalTime(static_cast<int32_t>(elapsedTimeSec_));

    // デスタイマー更新
    obj.StressGauge_->Update(timeSpeed);

    // ゲームオーバー時にコントローラ振動を一度だけ停止
    if (obj.StressGauge_->GetIsDeath() && !gameOverHandled_) {
        gameOverHandled_ = true;

        size_t numPads = KetaEngine::Input::GetNumberOfJoysticks();
        for (size_t i = 0; i < numPads; ++i) {
            KetaEngine::Input::SetVibration(static_cast<int32_t>(i), 0.0f, 0.0f);
        }
    }

    obj.enemyManager_->UIUpdate(pOwner_->GetViewProjection());
    obj.enemyManager_->SetViewProjection(&pOwner_->GetViewProjection());
    obj.lockOnController_->Update(obj.player_.get(), pOwner_->GetViewProjection());

    // ディザオクルージョン更新
    Vector3 playerPos = obj.player_->GetBaseTransform().GetWorldPos();
    pOwner_->GetDitherOcclusion()->Update(obj.gameCamera_->GetViewProjection(), playerPos);

    obj.comboSupportSpriteUi_->Update();
    obj.nextAttackHintUI_->Update();

    // ゲーム終了判定
    if (obj.StressGauge_->GetIsDeath() && obj.player_->GetIsDeathRenditionFinish()) {
        pOwner_->ChangeState(std::make_unique<GameSceneGameOver>(pOwner_));
        return;
    }

    ViewUpDate();
}

void GameScenePlaying::Debug() {
}
