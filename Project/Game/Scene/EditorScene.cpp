#include "EditorScene.h"
// base
#include "base/TextureManager.h"
// math
#include "2d/SpriteRegistry.h"
#include "Frame/Frame.h"
#include "Lighrt/Light.h"
#include "Scene/Manager/SceneManager.h"
#include <imgui.h>

void EditorScene::Init() {
    //// グローバル変数の読み込み
    KetaEngine::GlobalParameter::GetInstance()->LoadFiles();
    BaseScene::Init();
    // オブジェクト生成
    ObjectInit();

    // クラスポインタセット
    SetClassPointer();

    KetaEngine::Frame::ResetDeltaTime();
}

void EditorScene::Update() {
    BaseScene::Update();

    effectEditorSuite_->SetCameraPreViewPos(player_->GetWorldPosition());

    // Editor
    attackEffect_->Update();
    field_->Update();
    /*sideRopeController_->Update();*/
    audienceController_->Update();

    // 各クラス更新
    player_->Update();
    skyBox_->Update();
    enemyManager_->Update();
    combo_->Update();
    gameCamera_->Update();

    ViewProjectionUpdate();

    if (input_->TriggerKey(KeyboardKey::Enter)) {
        KetaEngine::SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
    }
}

/// ===================================================
/// SkyBox描画
/// ===================================================
void EditorScene::SkyBoxDraw() {
    skyBox_->Draw(viewProjection_);
}

void EditorScene::Debug() {
#ifdef _DEBUG
    ImGui::Begin("Camera");
    gameCamera_->AdjustParam();
    ImGui::End();

    BaseScene::Debug();

    KetaEngine::Light::GetInstance()->DebugImGui();

    ImGui::Begin("ParameterEditor");
    player_->AdjustParam();
    sideRopeController_->AdjustParam();
    enemyManager_->AdjustParam();
    enemySpawner_->AdjustParam();
    combo_->AdjustParam();
    audienceController_->AdjustParam();
    KetaEngine::ShadowMap::GetInstance()->DebugImGui();
    KetaEngine::SpriteRegistry::GetInstance()->DebugImGui();
    ImGui::End();

    ImGui::Begin("EnemySpawn");
    enemyManager_->DebugEnemySpawn();
    ImGui::End();

    ImGui::Begin("Rendition");
    attackEffect_->EditorUpdate();
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
void EditorScene::ViewProjectionUpdate() {
    BaseScene::ViewProjectionUpdate();
}

void EditorScene::ViewProcess() {
    viewProjection_.matView_       = gameCamera_->GetViewProjection().matView_;
    viewProjection_.matProjection_ = gameCamera_->GetViewProjection().matProjection_;
    viewProjection_.cameraMatrix_  = gameCamera_->GetViewProjection().cameraMatrix_;
    viewProjection_.rotation_      = gameCamera_->GetViewProjection().rotation_;
    viewProjection_.TransferMatrix();
}

void EditorScene::ObjectInit() {
    // gameObj_のメンバを初期化
    field_                       = std::make_unique<Field>();
    lockOnController_            = std::make_unique<LockOnController>();
    player_                      = std::make_unique<Player>();
    gameCamera_                  = std::make_unique<GameCamera>();
    enemyManager_                = std::make_unique<EnemyManager>();
    enemySpawner_                = std::make_unique<EnemySpawner>();
    skyBox_                      = std::make_unique<SkyBox>();
    combo_                       = std::make_unique<Combo>();
    attackEffect_                = std::make_unique<AttackEffect>();
    playerComboAttackController_ = std::make_unique<PlayerComboAttackController>();
    sideRopeController_          = std::make_unique<SideRopeController>();
    audienceController_          = std::make_unique<AudienceController>();
    ObjectFromBlender_           = std::make_unique<KetaEngine::ObjectFromBlender>();
    deathTimer_                  = std::make_unique<DeathTimer>();

    // 初期化
    player_->InitInGameScene();
    lockOnController_->Init();
    skyBox_->Init();
    combo_->Init();
    enemyManager_->Init();
    enemySpawner_->Init("enemySpawner.json");
    gameCamera_->Init();
    playerComboAttackController_->Init();
    attackEffect_->Init();
    sideRopeController_->Init();
    audienceController_->Init();
    deathTimer_->Init();
    viewProjection_.Init();

    ObjectFromBlender_->LoadJsonFile("gameScene.json");
}

void EditorScene::SetClassPointer() {
    gameCamera_->SetTarget(&player_->GetBaseTransform());

    enemyManager_->SetPlayer(player_.get());
    enemyManager_->SetCombo(combo_.get());
    enemyManager_->SetGameCamera(gameCamera_.get());
    enemyManager_->SetEnemySpawner(enemySpawner_.get());
    enemyManager_->SetAttackEffect(attackEffect_.get());

    enemySpawner_->SetEnemyManager(enemyManager_.get());

    lockOnController_->SetEnemyManager(enemyManager_.get());

    player_->SetViewProjection(&viewProjection_);
    player_->SetLockOn(lockOnController_.get());
    player_->SetGameCamera(gameCamera_.get());
    player_->SetComboAttackController(playerComboAttackController_.get());
    player_->SetCombo(combo_.get());
    player_->SetHitStop(attackEffect_.get());

    combo_->SetDeathTimer(deathTimer_.get());

    playerComboAttackController_->SetEditorSuite(effectEditorSuite_.get());
    playerComboAttackController_->SetPlayer(player_.get());
}
