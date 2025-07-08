#include "GameScene.h"
#include "Scene/Manager/SceneManager.h"
#include "utility/ParticleEditor/ParticleManager.h"
// base
#include "base/TextureManager.h"
// math
#include "Frame/Frame.h"
#include "JoyState/JoyState.h"
#include "Lighrt/Light.h"
#include <imgui.h>

GameScene::GameScene() {}

GameScene::~GameScene() {
}

void GameScene::Init() {
    //// グローバル変数の読み込み
    GlobalParameter::GetInstance()->LoadFiles();
    BaseScene::Init();

    ///=======================================================================================
    /// 生成
    ///=======================================================================================

    field_            = std::make_unique<Field>();
    lockOn_           = std::make_unique<LockOn>();
    player_           = std::make_unique<Player>();
    gamecamera_       = std::make_unique<GameCamera>();
    enemyManager_     = std::make_unique<EnemyManager>();
    enemySpawner_     = std::make_unique<EnemySpawner>();
    skydome_          = std::make_unique<Skydome>();
    howToOperate_     = std::make_unique<HowToOperate>();
    skyBox_           = std::make_unique<SkyBox>();
    combo_            = std::make_unique<Combo>();
    putObjForBlender_ = std::make_unique<PutObjForBlender>();
    comboCreate_      = std::make_unique<ComboCreateEditor>();
    fireInjectors_      = std::make_unique<FireInjectors>();

    ///=======================================================================================
    /// 初期化
    ///=======================================================================================

    field_->Init();
    skydome_->Init();
    player_->Init();
    lockOn_->Init();
    skyBox_->Init();
    combo_->Init();
    enemyManager_->Init();
    enemySpawner_->Init("enemySpawner.json");
    fireInjectors_->Init();
    gamecamera_->Init();
    howToOperate_->Init();
    viewProjection_.Init();

    putObjForBlender_->LoadJsonFile("gameScene.json");
    putObjForBlender_->PutObject();

    ///=======================================================================================
    /// セット
    ///=======================================================================================
    gamecamera_->SetTarget(&player_->GetTransform());
    enemyManager_->SetPlayer(player_.get());
    enemyManager_->SetCombo(combo_.get());
    enemyManager_->SetGameCamera(gamecamera_.get());
    enemyManager_->SetLockon(lockOn_.get());
    player_->SetViewProjection(&viewProjection_);
    player_->SetLockOn(lockOn_.get());
    player_->SetGameCamera(gamecamera_.get());
    enemySpawner_->SetEnemyManager(enemyManager_.get());
    fireInjectors_->SetCombo(combo_.get());

    enemyManager_->FSpawn();

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
      /*  cSprite_->SetPosition(Vector2(0, 0));*/

        // 　ジャンプに切り替え
        if (Input::GetInstance()->PushKey(DIK_SPACE)) {
            isend_ = true;
        } else {
            ChangeForJoyState();
        }
    });

  /*  cease_.time    = 0.0f;
    cease_.maxTime = 0.5f;*/
    chandle_       = TextureManager::GetInstance()->LoadTexture("Resources/Texture/Clear.png");
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
    skydome_->Update();
    skyBox_->Update();
    howToOperate_->Update();
    field_->Update();
    enemySpawner_->Update(Frame::DeltaTimeRate());
    enemyManager_->Update();
    combo_->Update();
    fireInjectors_->Update();
    gamecamera_->Update();

    //
    enemyManager_->HpBarUpdate(viewProjection_);
    lockOn_->Update(enemyManager_->GetEnemies(), viewProjection_);

    /// パーティクル更新
    ParticleManager::GetInstance()->Update();
    ViewProjectionUpdate();

    /// クリア
    if (enemyManager_->GetCread()/* && enemySpawner_->getos()*/) {
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

    ///// タイトルに戻る
    // if (input_->TrrigerKey(DIK_RETURN)) {
    //	SceneManager::GetInstance()->ChangeScene("TITLE");
    // }
}

/// ===================================================
/// モデル描画
/// ===================================================
void GameScene::ModelDraw() {
    /// commandList取得
    ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();
    Model::PreDraw(commandList);

    putObjForBlender_->DrawAll(viewProjection_);
    fireInjectors_->Draw(viewProjection_);

    field_->Draw(viewProjection_);
    player_->Draw(viewProjection_);
    enemyManager_->Draw(viewProjection_);


    ParticleManager::GetInstance()->Draw(viewProjection_);

    Model::PreDraw(commandList);
    player_->EffectDraw(viewProjection_);
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

void GameScene::Debug() {
#ifdef _DEBUG
    ImGui::Begin("Camera");
    gamecamera_->Debug();
    gamecamera_->AdjustParam();
    ImGui::End();

    
    Light::GetInstance()->DebugImGui();
    howToOperate_->Debug();

    ImGui::Begin("ParameterEditor");
    player_->AdjustParam();
    enemyManager_->AdjustParam();
    combo_->AdjustParam();
    fireInjectors_->AdjustParam();

    ImGui::End();
#endif
}

// ビュープロジェクション更新
void GameScene::ViewProjectionUpdate() {
    BaseScene::ViewProjectionUpdate();
}

void GameScene::ViewProssess() {
    viewProjection_.matView_       = gamecamera_->GetViewProjection().matView_;
    viewProjection_.matProjection_ = gamecamera_->GetViewProjection().matProjection_;
    viewProjection_.cameraMatrix_  = gamecamera_->GetViewProjection().cameraMatrix_;
    viewProjection_.rotation_      = gamecamera_->GetViewProjection().rotation_;
    viewProjection_.TransferMatrix();
}

void GameScene::ChangeForJoyState() {

    if (!((Input::IsTriggerPad(0, XINPUT_GAMEPAD_A))))
        return;

    isend_ = true;
}
