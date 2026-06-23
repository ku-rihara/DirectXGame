#include "GameScene.h"
// State
#include "GameSceneState/GameSceneIntro.h"
// Base
#include "Base/TextureManager.h"
// 2D
#include "2D/SpriteRegistry.h"
// Frame,Light
#include "Frame/Frame.h"
#include "Light/Light.h"
// ImGui
#include <imgui.h>

void GameScene::Init() {
    BaseScene::Init();
    // オブジェクト生成
    ObjectInit();

    // クラスポインタセット
    SetClassPointer();

    easingEditorFloat_.Init("EasingFloat");
    easingEditorVec2_.Init("EasingVec2");
    easingEditorVec3_.Init("EasingVec3");

    // 初期ステートを設定
    state_ = std::make_unique<GameSceneIntro>(this);
    state_->Init();

    KetaEngine::Frame::ResetDeltaTime();
}

void GameScene::Update() {
    BaseScene::Update();

    easingEditorFloat_.Update();
    easingEditorVec2_.Update();
    easingEditorVec3_.Update();

    // 現在のステートを更新
    if (state_) {
        state_->Update(KetaEngine::Frame::DeltaTimeRate());
    }
}

/// ===================================================
/// SkyBox描画
/// ===================================================
void GameScene::SkyBoxDraw() {
    gameObj_.skyBox_->Draw(viewProjection_);
}

void GameScene::Debug() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    ImGui::Begin("Camera");
    gameObj_.gameCamera_->AdjustParam();
    ImGui::End();

    BaseScene::Debug();

    KetaEngine::Light::GetInstance()->DebugImGui();
    gameObj_.operateUI_->Debug();

    ImGui::Begin("ParameterEditor");
    gameObj_.backGroundObjectManager_->AdjustParam();
    gameObj_.player_->AdjustParam();
    gameObj_.sideRopeController_->AdjustParam();
    gameObj_.enemyManager_->AdjustParam();
    gameObj_.continuousEnemySpawner_->AdjustParam();
    gameObj_.combo_->AdjustParam();
    gameObj_.fireInjectors_->AdjustParam();
    gameObj_.gameIntroManager_->AdjustParam();
    gameObj_.audienceController_->AdjustParam();
    gameObj_.lockOnController_->AdjustParam();
    gameObj_.StressGauge_->AdjustParam();
    gameObj_.killCounter_->AdjustParam();
    gameObj_.comboAssistController_->AdjustParam();
    gameObj_.unlockNotifier_->AdjustParam();
    gameObj_.comboSupportSpriteUi_->AdjustParam();
    gameObj_.nextAttackHintUI_->AdjustParam();
    gameObj_.operateUI_->Debug();
    KetaEngine::ShadowMap::GetInstance()->DebugImGui();
    KetaEngine::SpriteRegistry::GetInstance()->DebugImGui();
    ImGui::End();

    ImGui::Begin("EasingEditor");
    easingEditorFloat_.EditorUpdate();
    easingEditorVec2_.EditorUpdate();
    easingEditorVec3_.EditorUpdate();
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
    gameObj_.operateUI_                   = std::make_unique<OperateUI>();
    gameObj_.skyBox_                      = std::make_unique<SkyBox>();
    gameObj_.combo_                       = std::make_unique<Combo>();
    gameObj_.fireInjectors_               = std::make_unique<FireInjectors>();
    gameObj_.backGroundObjectManager_     = std::make_unique<BackGroundObjectManager>();
    gameObj_.comboDirector_               = std::make_unique<ComboDirector>();
    gameObj_.gameIntroManager_            = std::make_unique<GameIntroManager>();
    gameObj_.continuousEnemySpawner_      = std::make_unique<ContinuousEnemySpawner>();
    gameObj_.playerComboAttackController_ = std::make_unique<PlayerComboAttackController>();
    gameObj_.sideRopeController_          = std::make_unique<SideRopeController>();
    gameObj_.audienceController_          = std::make_unique<AudienceController>();
    gameObj_.StressGauge_                 = std::make_unique<StressGauge>();
    gameObj_.killCounter_                 = std::make_unique<KillCounter>();
    gameObj_.comboAssistController_       = std::make_unique<ComboAssistController>();
    gameObj_.unlockNotifier_              = std::make_unique<ComboUnlockNotifier>();
    gameObj_.comboSupportSpriteUi_        = std::make_unique<ComboSupportSpriteUi>();
    gameObj_.nextAttackHintUI_            = std::make_unique<NextAttackHintUI>();

    gameObj_.screenSprite_.reset(KetaEngine::Sprite::Create("screenChange.dds"));

    // 初期化
    gameObj_.field_->Init();
    gameObj_.player_->InitInGameScene();
    gameObj_.unlockNotifier_->Init();
    gameObj_.lockOnController_->Init();
    gameObj_.skyBox_->Init();
    gameObj_.combo_->Init();
    gameObj_.gameIntroManager_->Init();
    gameObj_.enemyManager_->Init();
    gameObj_.enemyManager_->InitSpawner("enemySpawner.json");
    gameObj_.continuousEnemySpawner_->Init();
    gameObj_.fireInjectors_->Init();
    gameObj_.gameCamera_->Init();
    gameObj_.operateUI_->Init();
    gameObj_.comboDirector_->Init();
    gameObj_.playerComboAttackController_->Init();
    gameObj_.killCounter_->SetAttackController(gameObj_.playerComboAttackController_.get());
    gameObj_.killCounter_->Init();
    gameObj_.sideRopeController_->Init();
    gameObj_.audienceController_->Init();
    gameObj_.StressGauge_->Init();
    viewProjection_.Init();
    gameObj_.nextAttackHintUI_->Init(gameObj_.player_.get());

    gameObj_.backGroundObjectManager_->Init();
}

void GameScene::SetClassPointer() {
    gameObj_.gameCamera_->Connect(&gameObj_);
    gameObj_.enemyManager_->Connect(&gameObj_);
    gameObj_.continuousEnemySpawner_->Connect(&gameObj_);
    gameObj_.fireInjectors_->Connect(&gameObj_);
    gameObj_.lockOnController_->Connect(&gameObj_);
    gameObj_.operateUI_->Connect(&gameObj_);
    gameObj_.gameIntroManager_->Connect(&gameObj_);
    gameObj_.comboDirector_->Connect(&gameObj_);
    gameObj_.player_->Connect(&gameObj_, viewProjection_);
    gameObj_.nextAttackHintUI_->Connect(viewProjection_);
    gameObj_.playerComboAttackController_->Connect(&gameObj_, effectEditorSuite_.get());
    gameObj_.comboAssistController_->Connect(&gameObj_);
    gameObj_.comboSupportSpriteUi_->Connect(&gameObj_);
    gameObj_.unlockNotifier_->Connect(&gameObj_);
}

void GameScene::ChangeState(std::unique_ptr<BaseGameSceneState> state) {
    state_ = std::move(state);
}
