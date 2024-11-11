#include "GameScene.h"
#include <cassert>
#include <imgui.h>
//class
#include"3d/Light.h"

GameScene::GameScene() {}

GameScene::~GameScene() {

}

void GameScene::Init() {
	// メンバ変数の初期化
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	textureManager_ = TextureManager::GetInstance();
	////////////////////////////////////////////////////////////////////////////////////////////
	//  生成
	////////////////////////////////////////////////////////////////////////////////////////////
	debugCamera_ = std::make_unique<DebugCamera>(1280, 720);
	controlPointManager_ = std::make_unique<ControlPointManager>();
	speedPointManager_ = std::make_unique<SpeedPointManager>();
	gameCamera_ = std::make_unique<GameCamera>();
	player_ = std::make_unique<Player>();
	enemyManager_ = std::make_unique<EnemyManager>();
	reticle_ = std::make_unique<Reticle>();
	skyDome_= std::make_unique<Skydome>();
	positionEditor_ = std::make_unique<PositionEditor>();
	score_ = std::make_unique<Score>();
	////////////////////////////////////////////////////////////////////////////////////////////
	//  初期化
	////////////////////////////////////////////////////////////////////////////////////////////
	debugCamera_->Init();
	gameCamera_->Init();
	controlPointManager_->Init();
	speedPointManager_->Init();
	enemyManager_->Init();
	viewProjection_.Init();
	player_->Init();
	reticle_->Init();
	skyDome_->Init();
	score_->Init();
	positionEditor_->Init();
	////////////////////////////////////////////////////////////////////////////////////////////
	//  セット
	////////////////////////////////////////////////////////////////////////////////////////////
	player_->SetReticle(reticle_.get());
	player_->SetGameCamera(gameCamera_.get());
	reticle_->SetPlayer(player_.get());
	enemyManager_->SetScore(score_.get());
	//parent
	player_->SetParent(&gameCamera_->GetWorldTransform());/// プレイヤーの親にゲームカメラ

}

void GameScene::Update() {
	
	
	Debug();//デバッグ

	//カメラ更新
	gameCamera_->Update(controlPointManager_->GetControlPoints());

	///========================================================================================
	///  オブジェ配置
	///========================================================================================
	positionEditor_->PutControlPoint(controlPointManager_.get());
	positionEditor_->PutEnemy(enemyManager_.get());

	ViewProjectionUpdate();

	///========================================================================================
	///  各クラス更新
	///========================================================================================
	
		//レールカメラ更新
	controlPointManager_->Update();
	/// 敵マネージャー更新
	enemyManager_->Update(player_->GetWorldPos());
	speedPointManager_->Update();
	//プレイヤー更新
	player_->Update();
	///レティクル更新
	reticle_->Updata(viewProjection_);

	skyDome_->Update();

	positionEditor_->Update(viewProjection_);

	score_->Update();

	
}

/// ===================================================
 /// モデル描画
 /// ===================================================
void GameScene::ModelDraw() {

	skyDome_->Draw(viewProjection_);

	controlPointManager_->Draw(viewProjection_);
	//プレイヤー
	if (isDebugCameraActive_) {
		player_->Draw(viewProjection_);
	}
	player_->BulletDraw(viewProjection_);
	////可視化オブジェクト
	positionEditor_->Draw(viewProjection_);
	///敵
	enemyManager_->Draw(viewProjection_);
	//レール
	gameCamera_->RailDraw(viewProjection_);

	speedPointManager_->Draw(viewProjection_);
}

/// ===================================================
 /// パーティクル描画
 /// ===================================================
void GameScene::ParticleDraw() {

}
/// ===================================================
/// スプライト描画
/// ===================================================
void GameScene::SpriteDraw() {
	//レティクル
	reticle_->Draw();
	player_->SpriteDraw();
	score_->Draw();

}

void GameScene::Debug() {
#ifdef _DEBUG
	ImGui::Begin("Window");

	if (ImGui::TreeNode("ViewProjection")) {

		ImGui::DragFloat3("Scale", &viewProjection_.scale_.x, 0.01f);
		ImGui::DragFloat3("Rotate", &viewProjection_.rotation_.x, 0.01f);
		ImGui::DragFloat3("Translate", &viewProjection_.translation_.x, 0.01f);
		ImGui::TreePop();
	}
	
	positionEditor_->Debug();
	player_->Debug();
	score_->Debug();
	gameCamera_->Debug();
	ImGui::End();
	//ライティング
	ImGui::Begin("Lighting");
	ImGui::End();
#endif
}


// ビュープロジェクション更新
void GameScene::ViewProjectionUpdate() {

#ifdef _DEBUG
	// デバッグカメラモード切り替え------------------------------
	if (input_->TrrigerKey(DIK_SPACE)) {
		if (isDebugCameraActive_ == false) {
			isDebugCameraActive_ = true;
		}
		else if (isDebugCameraActive_ == true) {
			isDebugCameraActive_ = false;
		}
	}
	// デバッグカメラモード切り替え------------------------------
#endif

	if (isDebugCameraActive_ == true) { // デバッグカメラがアクティブなら
		// デバッグカメラの更新
		debugCamera_->Update();
		// カメラ行列の計算をデバッグカメラのビュープロジェクションから行う
		viewProjection_.matView_ = debugCamera_->GetViewProjection().matView_;
		viewProjection_.matProjection_ = debugCamera_->GetViewProjection().matProjection_;
		viewProjection_.cameraMatrix_ = debugCamera_->GetViewProjection().cameraMatrix_;

	}
	// アクティブでない
	else if (isDebugCameraActive_ == false) { // デバッグカメラがアクティブでない
		viewProjection_.matView_ = gameCamera_->GetViewProjection().matView_;
		viewProjection_.matProjection_ = gameCamera_->GetViewProjection().matProjection_;
		viewProjection_.cameraMatrix_ = gameCamera_->GetViewProjection().cameraMatrix_;

		viewProjection_.TransferMatrix();
	}
}