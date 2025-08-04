#include "GameScene.h"
#include "3d/Object3DRegistry.h"
#include "Scene/Manager/SceneManager.h"
#include "utility/ParticleEditor/ParticleManager.h"
// base
#include "base/TextureManager.h"
// math
#include "Frame/Frame.h"
#include "Lighrt/Light.h"
#include <imgui.h>


void GameScene::Init() {
    //// グローバル変数の読み込み
    GlobalParameter::GetInstance()->LoadFiles();
    BaseScene::Init();

    ///=======================================================================================
    /// 生成
    ///=======================================================================================

    field_                = std::make_unique<Field>();
    lockOn_               = std::make_unique<LockOn>();
    player_               = std::make_unique<Player>();
    gameCamera_           = std::make_unique<GameCamera>();
    enemyManager_         = std::make_unique<EnemyManager>();
    enemySpawner_         = std::make_unique<EnemySpawner>();
    skyDome_              = std::make_unique<Skydome>();
    howToOperate_         = std::make_unique<HowToOperate>();
    skyBox_               = std::make_unique<SkyBox>();
    combo_                = std::make_unique<Combo>();
    gameBackGroundObject_ = std::make_unique<GameBackGroundObject>();
    comboCreate_          = std::make_unique<ComboCreateEditor>();
    fireInjectors_        = std::make_unique<FireInjectors>();

    ///=======================================================================================
    /// 初期化
    ///=======================================================================================

    player_->Init();
    lockOn_->Init();
    skyBox_->Init();
    combo_->Init();
    enemyManager_->Init();
    enemySpawner_->Init("enemySpawner.json");
    fireInjectors_->Init();
    gameCamera_->Init();
    howToOperate_->Init();
    viewProjection_.Init();

    gameBackGroundObject_->Init("game.json");

    ///=======================================================================================
    /// セット
    ///=======================================================================================
    gameCamera_->SetTarget(&player_->GetTransform());
    enemyManager_->SetPlayer(player_.get());
    enemyManager_->SetCombo(combo_.get());
    enemyManager_->SetGameCamera(gameCamera_.get());
    enemyManager_->SetLockOn(lockOn_.get());
    enemyManager_->SetEnemySpawner(enemySpawner_.get());
    player_->SetViewProjection(&viewProjection_);
    player_->SetLockOn(lockOn_.get());
    player_->SetGameCamera(gameCamera_.get());
    enemySpawner_->SetEnemyManager(enemyManager_.get());
    fireInjectors_->SetCombo(combo_.get());

    isfirstChange_ = false;
    alpha_         = 2.5f;
    shandle_       = TextureManager::GetInstance()->LoadTexture("Resources/Texture/screenChange.png");
    screenSprite_.reset(Sprite::Create(shandle_, Vector2(0, 0), Vector4(1, 1, 1, alpha_)));

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

    chandle_ = TextureManager::GetInstance()->LoadTexture("Resources/Texture/Clear.png");
    cSprite_.reset(Sprite::Create(chandle_, Vector2(0, -720), Vector4(1, 1, 1, 1.0f)));

    ParticleManager::GetInstance()->SetViewProjection(&viewProjection_);
}

void GameScene::Update() {

    screenSprite_->SetAlpha(alpha_);

    if (!isfirstChange_) {
        alpha_ -= Frame::DeltaTime();
        if (alpha_ <= 0.0f) {
            alpha_         = 0.0f;
            isfirstChange_ = true;
        }
    }

    // debugCamera
    debugCamera_->Update();
    Debug();

    // 各クラス更新
    player_->Update();
    skyBox_->Update();
    howToOperate_->Update();
    enemySpawner_->Update(Frame::DeltaTimeRate());
    enemyManager_->Update();
    combo_->Update();
    fireInjectors_->Update();
    gameCamera_->Update();
    if (isfirstChange_) {
        gameBackGroundObject_->Update();
    }

    enemyManager_->HpBarUpdate(viewProjection_);
    lockOn_->Update(enemyManager_->GetEnemies(), viewProjection_);

    
    Object3DRegistry::GetInstance()->UpdateAll();

    /// パーティクル更新
    ParticleManager::GetInstance()->Update();
    ViewProjectionUpdate();

    /// クリア
    if (enemyManager_->GetIsAllCleared() && enemySpawner_->GetAllGroupsCompleted()) {
        finishSpriteEase_.Update(Frame::DeltaTime());
        cSprite_->SetPosition(tempSpritePos_);
    }
    if (isend_) {
        alpha_ += Frame::DeltaTime();
        if (alpha_ >= 1.2f) {
            alpha_ = 1.0f;
            SceneManager::GetInstance()->ChangeScene("TITLE");
        }
    }
}

/// ===================================================
/// モデル描画
/// ===================================================
void GameScene::ModelDraw() {

    Object3DRegistry::GetInstance()->DrawAll(viewProjection_);
    ParticleManager::GetInstance()->Draw(viewProjection_);
}

/// ===================================================
/// SkyBox描画
/// ===================================================
void GameScene::SkyBoxDraw() {
    skyBox_->Draw(viewProjection_);
}

/// ======================================================
/// スプライト描画
/// ======================================================
void GameScene::SpriteDraw() {
    enemyManager_->SpriteDraw(viewProjection_);
    lockOn_->Draw();
    howToOperate_->Draw();
    combo_->Draw();
    cSprite_->Draw();
    screenSprite_->Draw();
}

/// ======================================================
/// 影描画
/// ======================================================
void GameScene::DrawShadow() {
    Object3DRegistry::GetInstance()->DrawAllShadow(viewProjection_);
}

void GameScene::Debug() {
#ifdef _DEBUG
    ImGui::Begin("Camera");
    gameCamera_->Debug();
    gameCamera_->AdjustParam();
    ImGui::End();

    Light::GetInstance()->DebugImGui();
    howToOperate_->Debug();

    ImGui::Begin("ParameterEditor");
    player_->AdjustParam();
    enemyManager_->AdjustParam();
    combo_->AdjustParam();
    fireInjectors_->AdjustParam();

    ShadowMap::GetInstance()->DebugImGui();

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
