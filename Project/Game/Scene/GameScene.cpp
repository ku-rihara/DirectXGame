#include "GameScene.h"
// state
#include "GameSceneState/GameSceneFinish.h"
#include "GameSceneState/GameSceneIntro.h"

// base
#include "base/TextureManager.h"
// math
#include "2d/SpriteRegistry.h"
#include "Frame/Frame.h"
#include "Light/Light.h"
#include "Scene/Manager/SceneManager.h"
#include <imgui.h>

void GameScene::Init() {
    //// グローバル変数の読み込み
    KetaEngine::GlobalParameter::GetInstance()->LoadFiles();
    BaseScene::Init();
    // オブジェクト生成
    ObjectInit();

    // クラスポインタセット
    SetClassPointer();

    // 初期ステートを設定
    state_ = std::make_unique<GameSceneIntro>(this);
    state_->Init();

    KetaEngine::Frame::ResetDeltaTime();
}

void GameScene::Update() {
    BaseScene::Update();

    // 現在のステートを更新
    if (state_) {
        state_->Update(KetaEngine::Frame::DeltaTimeRate());
    }

    ViewProjectionUpdate();

    if (GameSceneFinish* finishState = dynamic_cast<GameSceneFinish*>(state_.get())) {
        if (!finishState->GetIsGameEnd()) {
            return;
        }
        KetaEngine::SceneManager::GetInstance()->ChangeScene("TITLE");
    }
}

/// ===================================================
/// SkyBox描画
/// ===================================================
void GameScene::SkyBoxDraw() {
    gameObj_.skyBox_->Draw(viewProjection_);
}

void GameScene::Debug() {
#ifdef _DEBUG
    ImGui::Begin("Camera");
    gameObj_.gameCamera_->AdjustParam();
    ImGui::End();

    BaseScene::Debug();

    KetaEngine::Light::GetInstance()->DebugImGui();
    gameObj_.howToOperate_->Debug();

    ImGui::Begin("ParameterEditor");
    gameObj_.player_->AdjustParam();
    gameObj_.sideRopeController_->AdjustParam();
    gameObj_.enemyManager_->AdjustParam();
    gameObj_.enemySpawner_->AdjustParam();
    gameObj_.continuousEnemySpawner_->AdjustParam();
    gameObj_.combo_->AdjustParam();
    gameObj_.fireInjectors_->AdjustParam();
    gameObj_.gameIntroManager_->AdjustParam();
    gameObj_.audienceController_->AdjustParam();
    gameObj_.deathTimer_->AdjustParam();
    gameObj_.killCounter_->AdjustParam();
    KetaEngine::ShadowMap::GetInstance()->DebugImGui();
    KetaEngine::SpriteRegistry::GetInstance()->DebugImGui();
    ImGui::End();

    ImGui::Begin("Rendition");
    gameObj_.attackEffect_->EditorUpdate();
    ImGui::End();

    ImGui::Begin("PlayerAttack");
    gameObj_.playerComboAttackController_->EditorUpdate();
    ImGui::End();

    ImGui::Begin("EnemyDamageReaction");
    gameObj_.enemyManager_->DamageReactionCreate();
    ImGui::End();

    if (state_) {
        state_->Debug();
    }
#endif
}

// ビュープロジェクション更新
void GameScene::ViewProjectionUpdate() {
    BaseScene::ViewProjectionUpdate();
}

void GameScene::ViewProcess() {
    viewProjection_.matView_       = gameObj_.gameCamera_->GetViewProjection().matView_;
    viewProjection_.matProjection_ = gameObj_.gameCamera_->GetViewProjection().matProjection_;
    viewProjection_.cameraMatrix_  = gameObj_.gameCamera_->GetViewProjection().cameraMatrix_;
    viewProjection_.rotation_      = gameObj_.gameCamera_->GetViewProjection().rotation_;
    viewProjection_.TransferMatrix();
}

void GameScene::ObjectInit() {
    // gameObj_のメンバを初期化
    gameObj_.field_                       = std::make_unique<Field>();
    gameObj_.lockOnController_            = std::make_unique<LockOnController>();
    gameObj_.player_                      = std::make_unique<Player>();
    gameObj_.gameCamera_                  = std::make_unique<GameCamera>();
    gameObj_.enemyManager_                = std::make_unique<EnemyManager>();
    gameObj_.enemySpawner_                = std::make_unique<EnemySpawner>();
    gameObj_.howToOperate_                = std::make_unique<HowToOperate>();
    gameObj_.skyBox_                      = std::make_unique<SkyBox>();
    gameObj_.combo_                       = std::make_unique<Combo>();
    gameObj_.fireInjectors_               = std::make_unique<FireInjectors>();
    gameObj_.gameBackGroundObject_        = std::make_unique<GameBackGroundObject>();
    gameObj_.comboDirector_                  = std::make_unique<ComboDirector>();
    gameObj_.attackEffect_                = std::make_unique<AttackEffect>();
    gameObj_.gameIntroManager_            = std::make_unique<GameIntroManager>();
    gameObj_.comboLevelObjHolder_         = std::make_unique<ComboLevelObjHolder>();
    gameObj_.continuousEnemySpawner_      = std::make_unique<ContinuousEnemySpawner>();
    gameObj_.playerComboAttackController_ = std::make_unique<PlayerComboAttackController>();
    gameObj_.sideRopeController_          = std::make_unique<SideRopeController>();
    gameObj_.audienceController_          = std::make_unique<AudienceController>();
    gameObj_.deathTimer_                  = std::make_unique<DeathTimer>();
    gameObj_.killCounter_                 = std::make_unique<KillCounter>();

    gameObj_.screenSprite_.reset(KetaEngine::Sprite::Create("screenChange.dds"));

    // 初期化
    gameObj_.player_->InitInGameScene();
    gameObj_.lockOnController_->Init();
    gameObj_.skyBox_->Init();
    gameObj_.combo_->Init();
    gameObj_.gameIntroManager_->Init();
    gameObj_.enemyManager_->Init();
    gameObj_.enemySpawner_->Init("enemySpawner.json");
    gameObj_.continuousEnemySpawner_->Init();
    gameObj_.fireInjectors_->Init();
    gameObj_.gameCamera_->Init();
    gameObj_.howToOperate_->Init();
    gameObj_.comboDirector_->Init();
    gameObj_.playerComboAttackController_->Init();
    gameObj_.killCounter_->SetAttackController(gameObj_.playerComboAttackController_.get());
    gameObj_.killCounter_->Init();
    gameObj_.attackEffect_->Init();
    gameObj_.sideRopeController_->Init();
    gameObj_.audienceController_->Init();
    gameObj_.deathTimer_->Init();
    viewProjection_.Init();

    gameObj_.comboLevelObjHolder_->Add(ComboLevelObjType::STADIUM_LIGHT, "ComboLevel1.json");
    gameObj_.comboLevelObjHolder_->Add(ComboLevelObjType::SPEAKER, "ComboLevel2.json");
    gameObj_.gameBackGroundObject_->Init("game.json");
}

void GameScene::SetClassPointer() {
    gameObj_.gameCamera_->SetTarget(&gameObj_.player_->GetBaseTransform());

    gameObj_.enemyManager_->SetPlayer(gameObj_.player_.get());
    gameObj_.enemyManager_->SetCombo(gameObj_.combo_.get());
    gameObj_.enemyManager_->SetKillCounter(gameObj_.killCounter_.get());
    gameObj_.enemyManager_->SetGameCamera(gameObj_.gameCamera_.get());
    gameObj_.enemyManager_->SetEnemySpawner(gameObj_.enemySpawner_.get());
  
    gameObj_.enemySpawner_->SetEnemyManager(gameObj_.enemyManager_.get());
    gameObj_.continuousEnemySpawner_->SetEnemyManager(gameObj_.enemyManager_.get());
    gameObj_.continuousEnemySpawner_->SetPlayer(gameObj_.player_.get());

    gameObj_.fireInjectors_->SetCombo(gameObj_.combo_.get());
    gameObj_.lockOnController_->SetEnemyManager(gameObj_.enemyManager_.get());

    gameObj_.gameIntroManager_->SetFireInjectors(gameObj_.fireInjectors_.get());
    gameObj_.gameIntroManager_->SetGameCamera(gameObj_.gameCamera_.get());
    gameObj_.gameIntroManager_->SetPlayer(gameObj_.player_.get());
    gameObj_.gameIntroManager_->SetGameBackGroundObject(gameObj_.gameBackGroundObject_.get());
    gameObj_.gameIntroManager_->SetHowToOperate(gameObj_.howToOperate_.get());
    gameObj_.gameIntroManager_->SetDeathTimerGauge(gameObj_.deathTimer_->GetDeathTimerGauge());
    gameObj_.gameIntroManager_->ClassisSet();

    gameObj_.comboDirector_->SetPlayer(gameObj_.player_.get());
    gameObj_.comboDirector_->SetComboAndDeathTimer(gameObj_.combo_.get(), gameObj_.deathTimer_.get());
    gameObj_.comboDirector_->SetComboLevelObjHolder(gameObj_.comboLevelObjHolder_.get());
    gameObj_.comboDirector_->SetAudienceController(gameObj_.audienceController_.get());

    gameObj_.player_->SetViewProjection(&viewProjection_);
    gameObj_.player_->SetLockOn(gameObj_.lockOnController_.get());
    gameObj_.player_->SetGameCamera(gameObj_.gameCamera_.get());
    gameObj_.player_->SetComboAttackController(gameObj_.playerComboAttackController_.get());
    gameObj_.player_->SetCombo(gameObj_.combo_.get());
    gameObj_.player_->SetHitStop(gameObj_.attackEffect_.get());
    gameObj_.player_->SetDeathTimer(gameObj_.deathTimer_.get());
    gameObj_.player_->SetDeathFragPointer(&gameObj_.deathTimer_->GetIsDeath());

    gameObj_.playerComboAttackController_->SetEditorSuite(effectEditorSuite_.get());
    gameObj_.playerComboAttackController_->SetPlayer(gameObj_.player_.get());
}

void GameScene::ChangeState(std::unique_ptr<BaseGameSceneState> state) {
    state_ = std::move(state);
}