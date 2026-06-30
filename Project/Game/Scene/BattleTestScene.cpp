#include "BattleTestScene.h"
// base
// math
#include "2D/SpriteRegistry.h"
#include "Frame/Frame.h"
#include "Light/Light.h"
#include "Scene/Manager/SceneManager.h"
#include <imgui.h>

void BattleTestScene::Init() {
    //// グローバル変数の読み込み
    KetaEngine::GlobalParameter::GetInstance()->LoadFiles();
    BaseScene::Init();
    // オブジェクト生成
    ObjectInit();

    // クラスポインタセット
    SetClassPointer();

    KetaEngine::Frame::ResetDeltaTime();
}

void BattleTestScene::Update() {
    BaseScene::Update();

    effectEditorSuite_->SetCameraPreViewPos(player_->GetWorldPosition());

    // UI更新
    comboAssistController_->Update();
    unlockNotifier_->Update(KetaEngine::Frame::DeltaTime());

    // Editor
    field_->Update();
    audienceController_->Update();

    // 各クラス更新
    player_->Update();
    skyBox_->Update();
    enemyManager_->Update();
    combo_->Update();
    comboDirector_->Update();
    backGroundObjectManager_->Update(KetaEngine::Frame::DeltaTimeRate());
    gameCamera_->Update();
    lockOnController_->Update(player_.get(), viewProjection_);

    ViewProjectionUpdate();

    if (input_->TriggerKey(KeyboardKey::Enter)) {
        KetaEngine::SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
    }
}

/// ===================================================
/// SkyBox描画
/// ===================================================
void BattleTestScene::SkyBoxDraw() {
    skyBox_->Draw(viewProjection_);
}

void BattleTestScene::Debug() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    ImGui::Begin("Camera");
    gameCamera_->AdjustParam();
    ImGui::End();

    BaseScene::Debug();

    KetaEngine::Light::GetInstance()->DebugImGui();

    ImGui::Begin("ParameterEditor");
    player_->AdjustParam();
    sideRopeController_->AdjustParam();
    enemyManager_->AdjustParam();
    combo_->AdjustParam();
    killCounter_->AdjustParam();
    comboAssistController_->AdjustParam();
    unlockNotifier_->AdjustParam();
    audienceController_->AdjustParam();
    backGroundObjectManager_->AdjustParam();
    lockOnController_->AdjustParam();
    KetaEngine::ShadowMap::GetInstance()->DebugImGui();
    KetaEngine::SpriteRegistry::GetInstance()->DebugImGui();
    ImGui::End();

    ImGui::Begin("EnemySpawn");
    enemyManager_->DebugEnemySpawn();
    ImGui::End();

    ImGui::Begin("PlayerAttack");
    playerComboAttackController_->EditorUpdate();
    ImGui::End();

    ImGui::Begin("EnemyDamageReaction");
    enemyManager_->DamageReactionCreate();
    ImGui::End();

#endif
}

// ビュープロジェクション更新
void BattleTestScene::ViewProjectionUpdate() {
    BaseScene::ViewProjectionUpdate();
}

void BattleTestScene::ViewProcess() {
    viewProjection_.matView_       = gameCamera_->GetViewProjection().matView_;
    viewProjection_.matProjection_ = gameCamera_->GetViewProjection().matProjection_;
    viewProjection_.cameraMatrix_  = gameCamera_->GetViewProjection().cameraMatrix_;
    viewProjection_.rotation_      = gameCamera_->GetViewProjection().rotation_;
    viewProjection_.TransferMatrix();
}

void BattleTestScene::ObjectInit() {
    // gameObj_のメンバを初期化
    field_                       = std::make_unique<Field>();
    lockOnController_            = std::make_unique<LockOnController>();
    player_                      = std::make_unique<Player>();
    gameCamera_                  = std::make_unique<GameCamera>();
    enemyManager_                = std::make_unique<EnemyManager>();
    skyBox_                      = std::make_unique<SkyBox>();
    combo_                       = std::make_unique<Combo>();
    playerComboAttackController_ = std::make_unique<PlayerComboAttackController>();
    sideRopeController_          = std::make_unique<SideRopeController>();
    audienceController_          = std::make_unique<AudienceController>();
    backGroundObjectManager_     = std::make_unique<BackGroundObjectManager>();
    comboDirector_               = std::make_unique<ComboDirector>();
    StressGauge_                 = std::make_unique<StressGauge>();
    killCounter_                 = std::make_unique<KillCounter>();
    comboAssistController_        = std::make_unique<ComboAssistController>();
    unlockNotifier_              = std::make_unique<ComboUnlockNotifier>();
    operateUI_                   = std::make_unique<OperateUI>();
    unlockNotifier_->Init();
    operateUI_->Init();

    // 初期化
    field_->Init();
    player_->Init();
    player_->SetIsIgnoreUnlockState(true);
    lockOnController_->Init();
    skyBox_->Init();
    combo_->Init();
    comboDirector_->Init();
    backGroundObjectManager_->Init();
    enemyManager_->Init();
    enemyManager_->InitSpawner("enemySpawner.json");
    gameCamera_->Init();
    playerComboAttackController_->Init();
    killCounter_->SetAttackController(playerComboAttackController_.get());
    killCounter_->Init();
    sideRopeController_->Init();
    audienceController_->Init();
    StressGauge_->Init();
    viewProjection_.Init();
}

void BattleTestScene::SetClassPointer() {
    gameCamera_->SetTarget(&player_->GetBaseTransform());

    enemyManager_->SetPlayer(player_.get());
    enemyManager_->SetCombo(combo_.get());
    enemyManager_->SetKillCounter(killCounter_.get());
    enemyManager_->SetGameCamera(gameCamera_.get());
    lockOnController_->SetEnemyManager(enemyManager_.get());

    player_->SetViewProjection(&viewProjection_);
    player_->SetLockOn(lockOnController_.get());
    gameCamera_->SetLockOn(lockOnController_->GetLockOn());
    player_->SetGameCamera(gameCamera_.get());
    player_->SetComboAttackController(playerComboAttackController_.get());
    player_->SetCombo(combo_.get());
    player_->SetStressGauge(StressGauge_.get());
    player_->SetDeathFragPointer(&StressGauge_->GetIsDeath());

    combo_->SetStressGauge(StressGauge_.get());

    comboDirector_->SetPlayer(player_.get());
    comboDirector_->SetComboAndStressGauge(combo_.get(), StressGauge_.get());
    comboDirector_->SetComboLevelObjHolder(backGroundObjectManager_->GetComboLevelObjHolder());
    comboDirector_->SetAudienceController(audienceController_.get());

    playerComboAttackController_->SetEditorSuite(effectEditorSuite_.get());
    playerComboAttackController_->SetPlayer(player_.get());

    operateUI_->SetPlayer(player_.get());

    comboAssistController_->SetAttackController(playerComboAttackController_.get());
    comboAssistController_->SetPlayer(player_.get());
    comboAssistController_->Init();

    // 自動コンボ実行 → アンロック通知UIリアクション の接続
    ComboUnlockNotifier* notifier = unlockNotifier_.get();
    player_->SetAutoComboAttackCallback([notifier](const std::string& name) {
        notifier->NotifyAttackExecuted(name);
    });

    // 攻撃解放 → アンロック通知UI の接続
    {
        ComboAssistController* assist          = comboAssistController_.get();
        PlayerComboAttackController* atkCtrl = playerComboAttackController_.get();
        Player* playerPtr                    = player_.get();
        killCounter_->SetOnAttackUnlockedCallback([notifier, assist, atkCtrl, playerPtr](const std::string& name) {
            notifier->OnAttackUnlocked(name, assist->GetLayoutParam(), atkCtrl, playerPtr);
        });
    }
}
