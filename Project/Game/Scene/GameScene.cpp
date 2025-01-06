#include "GameScene.h"
#include"Scene/Manager/SceneManager.h"
#include"utility/Particle/ParticleManager.h"
// base
#include "base/TextureManager.h"
//math
#include<imgui.h>
#include"3d/Light.h"

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

	///=======================================================================================
	/// 初期化
	///=======================================================================================
	field_->Init();
	skydome_->Init();
	player_->Init();
	lockOn_->Init();
	enemyManager_->Init();
	gamecamera_->Init();


	viewProjection_.Init();//ビュープロジェクション


	gamecamera_->SetTarget(&player_->GetTransform());
	enemyManager_->SetPlayer(player_.get());
	player_->SetViewProjection(&viewProjection_);
	player_->SetLockOn(lockOn_.get());


}

void GameScene::Update() {


	/// debugcamera
	debugCamera_->Update();
	Debug();

	//各クラス更新
	player_->Update();
	skydome_->Update();
	field_->Update();
	enemyManager_->Update();
	gamecamera_->Update();

	enemyManager_->HpBarUpdate(viewProjection_);
	lockOn_->Update(enemyManager_->GetEnemies(), viewProjection_);

	/// パーティクル更新
	ParticleManager::GetInstance()->Update(&viewProjection_);
	
	ViewProjectionUpdate();

	/// タイトルに戻る
	if (input_->TrrigerKey(DIK_RETURN)) {
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}
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
}

void GameScene::Debug() {
#ifdef _DEBUG
	ImGui::Begin("Camera");
	gamecamera_->Debug();
	ImGui::End();

	enemyManager_->ImGuiUpdate();
	Light::GetInstance()->DebugImGui();
	

	ImGui::Begin("ParamaterEditor");
	player_->AdjustParm();
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

	viewProjection_.TransferMatrix();

}