#include "GameScene.h"
#include "3d/Object3DRegistry.h"
#include "Scene/Manager/SceneManager.h"
#include "utility/ParticleEditor/ParticleManager.h"
// base
#include "base/TextureManager.h"
// math
#include "Animation/AnimationRegistry.h"
#include "Frame/Frame.h"
#include "Lighrt/Light.h"
#include <imgui.h>
#include "2d/SpriteRegistry.h"

void GameScene::Init() {
    //// グローバル変数の読み込み
    GlobalParameter::GetInstance()->LoadFiles();
    BaseScene::Init();

    ///=======================================================================================
    /// 生成
    ///=======================================================================================

    field_                = std::make_unique<Field>();
    lockOnController_     = std::make_unique<LockOnController>();
    player_               = std::make_unique<Player>();
    gameCamera_           = std::make_unique<GameCamera>();
    enemyManager_         = std::make_unique<EnemyManager>();
    enemySpawner_         = std::make_unique<EnemySpawner>();
    skyDome_              = std::make_unique<Skydome>();
    howToOperate_         = std::make_unique<HowToOperate>();
    skyBox_               = std::make_unique<SkyBox>();
    combo_                = std::make_unique<Combo>();
    fireInjectors_        = std::make_unique<FireInjectors>();
    gameBackGroundObject_ = std::make_unique<GameBackGroundObject>();
    comboScene_           = std::make_unique<ComboScene>();
    attackEffect_         = std::make_unique<AttackEffect>();
    gameIntroManager_     = std::make_unique<GameIntroManager>();

    ///=======================================================================================
    /// 初期化
    ///=======================================================================================

    player_->GameSceneInit();
    lockOnController_->Init();
    skyBox_->Init();
    combo_->Init();
    gameIntroManager_->Init();
    enemyManager_->Init();
    enemySpawner_->Init("enemySpawner.json");
    fireInjectors_->Init();
    gameCamera_->Init();
    howToOperate_->Init();
    comboScene_->Init();
    attackEffect_->Init();
    viewProjection_.Init();

    gameBackGroundObject_->Init("game.json");

    ///---------------------------------------------------------------------------------------
    /// セット
    ///---------------------------------------------------------------------------------------

    gameCamera_->SetTarget(&player_->GetTransform());
    enemyManager_->SetPlayer(player_.get());
    enemyManager_->SetCombo(combo_.get());
    enemyManager_->SetGameCamera(gameCamera_.get());
    enemyManager_->SetEnemySpawner(enemySpawner_.get());
    enemyManager_->SetAttackEffect(attackEffect_.get());
    player_->SetViewProjection(&viewProjection_);
    player_->SetLockOn(lockOnController_.get());
    player_->SetGameCamera(gameCamera_.get());
    player_->SetCombo(combo_.get());
    player_->SetHitStop(attackEffect_.get());
    enemySpawner_->SetEnemyManager(enemyManager_.get());
    fireInjectors_->SetCombo(combo_.get());
    lockOnController_->SetEnemyManager(enemyManager_.get());
    // gameIntro
    gameIntroManager_->SetFireInjectors(fireInjectors_.get());
    gameIntroManager_->SetGameCamera(gameCamera_.get());
    gameIntroManager_->SetPlayer(player_.get());
    gameIntroManager_->SetGameBackGroundObject(gameBackGroundObject_.get());
    gameIntroManager_->SetHowToOperate(howToOperate_.get());
    gameIntroManager_->ClassisSet();

    // comboScene
    comboScene_->SetPlayer(player_.get());
    comboScene_->SetCombo(combo_.get());

    isfirstChange_ = false;
    shandle_       = TextureManager::GetInstance()->LoadTexture("Resources/Texture/screenChange.png");
    screenSprite_.reset(Sprite::Create("screenChange.png"));

    finishSpriteEase_.Init("finishSpriteTest");
    finishSpriteEase_.ApplyFromJson("finishSpriteTest.json");
    finishSpriteEase_.SaveAppliedJsonFileName();
    finishSpriteEase_.SetAdaptValue(&tempSpritePos_);
    finishSpriteEase_.Reset();

    finishSpriteEase_.SetOnFinishCallback([this]() {
        // 　ジャンプに切り替え
        if (Input::GetInstance()->PushKey(DIK_SPACE)) {
            isend_ = true;
        } else {
            ChangeForJoyState();
        }
    });

    cSprite_.reset(Sprite::Create("Clear.png"));
    cSprite_->transform_.pos = Vector2(0, -720);

    ParticleManager::GetInstance()->SetViewProjection(&viewProjection_);

    gameState_ = GameState::INTRO;
}

void GameScene::Update() {

    switch (gameState_) {
    case GameScene::GameState::INTRO:

        IntroUpdate();

        if (gameIntroManager_->GetIsFinishStep(GameIntroManager::AppearPurpose)) {
            gameState_ = GameScene::GameState::PLAY;
        }

        break;
    case GameScene::GameState::PLAY:

        PlayUpdate();

        /// クリア
        if (enemyManager_->GetIsAllCleared() && enemySpawner_->GetAllGroupsCompleted()) {
            gameState_ = GameScene::GameState::FINISH;
        }

        break;
    case GameScene::GameState::FINISH:

        FinishUpdate();

        break;
    default:
        break;
    }

    Object3DRegistry::GetInstance()->UpdateAll();

    /// パーティクル更新
    ParticleManager::GetInstance()->Update();
    ViewProjectionUpdate();

    if (alpha_ >= 1.2f) {
        alpha_ = 1.0f;
        SceneManager::GetInstance()->ChangeScene("TITLE");
    }
}

void GameScene::IntroUpdate() {

    if (Frame::DeltaTime() >= 2.0f) {
        return;
    }

    screenSprite_->SetAlpha(alpha_);

    if (!isfirstChange_) {
        alpha_ -= Frame::DeltaTime();

        if (alpha_ <= 0.0f) {
            alpha_         = 0.0f;
            isfirstChange_ = true;
        }
    }

    // gameIntro
    gameIntroManager_->Update();
    howToOperate_->Update();
    if (gameIntroManager_->GetIsFinishStep(GameIntroManager::SpawnField)) {
        enemySpawner_->Update(Frame::DeltaTimeRate());
        enemyManager_->Update();
        enemyManager_->HpBarUpdate(viewProjection_);
    }
    // debugCamera
    debugCamera_->Update();

    // Editor
    attackEffect_->Update();
    Debug();

    // obj
    skyBox_->Update();
    gameCamera_->Update(gameIntroManager_->GetCurrentPlaySpeedRate());
    combo_->Update();
}
void GameScene::PlayUpdate() {

    // debugCamera
    debugCamera_->Update();
   
    // Editor
    attackEffect_->Update();
    Debug();

    // 各クラス更新
    comboScene_->Update();
    player_->Update();
    skyBox_->Update();
    howToOperate_->Update();
    enemySpawner_->Update(Frame::DeltaTimeRate());
    enemyManager_->Update();
    combo_->Update();
    fireInjectors_->Update();
    gameCamera_->Update();

    enemyManager_->HpBarUpdate(viewProjection_);
    lockOnController_->Update(player_.get(), viewProjection_);
}
void GameScene::FinishUpdate() {

    screenSprite_->SetAlpha(alpha_);

    finishSpriteEase_.Update(Frame::DeltaTime());
    cSprite_->transform_.pos = tempSpritePos_;

    if (!isend_) {
        return;
    }

    alpha_ += Frame::DeltaTime();
}


/// ===================================================
/// SkyBox描画
/// ===================================================
void GameScene::SkyBoxDraw() {
    skyBox_->Draw(viewProjection_);
}

void GameScene::Debug() {
#ifdef _DEBUG
    ImGui::Begin("Camera");
    
    gameCamera_->AdjustParam();
    ImGui::End();

    Light::GetInstance()->DebugImGui();
    howToOperate_->Debug();

    ImGui::Begin("ParameterEditor");
    player_->AdjustParam();
    enemyManager_->AdjustParam();
    combo_->AdjustParam();
    fireInjectors_->AdjustParam();
    gameIntroManager_->AdjustParam();
    ShadowMap::GetInstance()->DebugImGui();
    SpriteRegistry::GetInstance()->DebugImGui();
    ImGui::End();

    ImGui::Begin("Rendition");

    attackEffect_->EditorUpdate();
    ImGui::End();
#endif
}

// ビュープロジェクション更新
void GameScene::ViewProjectionUpdate() {
    BaseScene::ViewProjectionUpdate();
}

void GameScene::ViewProssess() {
    viewProjection_.matView_       = gameCamera_->GetViewProjection().matView_;
    viewProjection_.matProjection_ = gameCamera_->GetViewProjection().matProjection_;
    viewProjection_.cameraMatrix_  = gameCamera_->GetViewProjection().cameraMatrix_;
    viewProjection_.rotation_      = gameCamera_->GetViewProjection().rotation_;
    viewProjection_.TransferMatrix();
}

void GameScene::ChangeForJoyState() {

    if (!((Input::IsTriggerPad(0, XINPUT_GAMEPAD_A)))) {
        return;
    }

    isend_ = true;
}
