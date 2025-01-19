#include "GameScene.h"
#include"Scene/Manager/SceneManager.h"
#include"utility/Particle/ParticleManager.h"
// base
#include "base/TextureManager.h"
//math
#include<imgui.h>
#include"Lighrt/Light.h"
#include"Frame/Frame.h"
#include"JoyState/JoyState.h"

GameScene::GameScene() {}

GameScene::~GameScene() {

}

void GameScene::Init() {

	BaseScene::Init();
	

	///=======================================================================================
	/// 生成
	///=======================================================================================

	field_ = std::make_unique<Field>();
	lockOn_ = std::make_unique<LockOn>();
	player_ = std::make_unique<Player>();
	gamecamera_ = std::make_unique<GameCamera>();
	enemyManager_ = std::make_unique<EnemyManager>();
	skydome_= std::make_unique<Skydome>();
	howToOperate_ = std::make_unique<HowToOperate>();

	///=======================================================================================
	/// 初期化
	///=======================================================================================
	field_->Init();
	skydome_->Init();
	player_->Init();
	lockOn_->Init();
	enemyManager_->Init();
	gamecamera_->Init();
	howToOperate_->Init();

	viewProjection_.Init();//ビュープロジェクション

	gamecamera_->SetTarget(&player_->GetTransform());
	enemyManager_->SetPlayer(player_.get());
	player_->SetViewProjection(&viewProjection_); 
	player_->SetLockOn(lockOn_.get());
	player_->SetGameCamera(gamecamera_.get());
	enemyManager_->SetGameCamera(gamecamera_.get());
	enemyManager_->SetLockon(lockOn_.get());

	enemyManager_->FSpawn();

	isfirstChange_ = false;
	alpha_ = 2.5f;
	shandle_ = TextureManager::GetInstance()->LoadTexture("./resources/Texture/screenChange.png");
	screenSprite_.reset(Sprite::Create(shandle_, Vector2(0, 0), Vector4(1, 1, 1, alpha_)));

	cease_.time = 0.0f;
	cease_.maxTime = 0.5f;
	chandle_ = TextureManager::GetInstance()->LoadTexture("./resources/Texture/Clear.png");
	cSprite_.reset(Sprite::Create(chandle_, Vector2(0, -720), Vector4(1, 1, 1, 1.0f)));
}

void GameScene::Update() {

	screenSprite_->SetAlpha(alpha_);

	if (!isfirstChange_) {
		alpha_ -= Frame::DeltaTime();
		if (alpha_ <= 0.0f) {
			alpha_ = 0.0f;
			isfirstChange_ = true;
		}
	}

	/// debugcamera
	debugCamera_->Update();
	Debug();
	
	//各クラス更新
	player_->Update();
	skydome_->Update();
	howToOperate_->Update();
	field_->Update();
	enemyManager_->Update();
	gamecamera_->Update();

	//
	enemyManager_->HpBarUpdate(viewProjection_);
	lockOn_->Update(enemyManager_->GetEnemies(), viewProjection_);


	/// パーティクル更新
	ParticleManager::GetInstance()->Update(&viewProjection_);
	
	ViewProjectionUpdate();

	/// クリア
	if (enemyManager_->GetCread()) {
		cease_.time += Frame::DeltaTime();
	
		if (cease_.time >= cease_.maxTime) {
			cease_.time = cease_.maxTime;
			cSprite_->SetPosition(Vector2(0, 0));
		
			//　ジャンプに切り替え
			if (Input::GetInstance()->PushKey(DIK_SPACE)) {
				isend_ = true;
			}
			else {
				ChangeForJoyState();//コントローラジャンプ
			}

		}
		else {
			cSprite_->SetPosition(Vector2(0, EaseInCubic(-720.0f, 0.0f, cease_.time, cease_.maxTime)));

		}
	}
	if (isend_) {
		alpha_ += Frame::DeltaTime();
		if (alpha_ >= 1.2f) {
			alpha_ = 1.0f;
			SceneManager::GetInstance()->ChangeScene("TITLE");
		}
	}

	///// タイトルに戻る
	//if (input_->TrrigerKey(DIK_RETURN)) {
	//	SceneManager::GetInstance()->ChangeScene("TITLE");
	//}
}

/// ===================================================
/// モデル描画
/// ===================================================
void GameScene::ModelDraw() {

	//平面描画
	if (isDraw) {
		skydome_->Draw(viewProjection_);
		field_->Draw(viewProjection_);
		enemyManager_->Draw(viewProjection_);
		player_->Draw(viewProjection_);
		
	}
}

/// ===================================================
   /// パーティクル描画
   /// ===================================================
void GameScene::ParticleDraw() {
	ParticleManager::GetInstance()->Draw(viewProjection_);
}

/// ===================================================
   /// スプライト描画
   /// ===================================================
void GameScene::SpriteDraw() {
	enemyManager_->SpriteDraw(viewProjection_);
	lockOn_->Draw();
	howToOperate_->Draw();
	cSprite_->Draw();
	screenSprite_->Draw();
}

void GameScene::Debug() {
#ifdef _DEBUG
	ImGui::Begin("Camera");
	gamecamera_->Debug();
	ImGui::End();

	enemyManager_->ImGuiUpdate();
	Light::GetInstance()->DebugImGui();
	howToOperate_->Debug();
	

	ImGui::Begin("ParamaterEditor");
	player_->AdjustParm();
	enemyManager_->AdjustParm();
	ImGui::End();
#endif
}


// ビュープロジェクション更新
void GameScene::ViewProjectionUpdate() {
	BaseScene::ViewProjectionUpdate();
}


void GameScene::ViewProssess() {
	viewProjection_.matView_ = gamecamera_->GetViewProjection().matView_;
	viewProjection_.matProjection_ = gamecamera_->GetViewProjection().matProjection_;
	viewProjection_.cameraMatrix_ = gamecamera_->GetViewProjection().cameraMatrix_;
	viewProjection_.rotation_ = gamecamera_->GetViewProjection().rotation_;
	viewProjection_.TransferMatrix();
	
}

void  GameScene::ChangeForJoyState() {
	if (!(Input::GetInstance()->GetJoystickState(0, joyState))) return;

	if (!((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A))) return;

	isend_ = true;

}
