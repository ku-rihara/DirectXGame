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
	railManager_ = std::make_unique<RailManager>();
	gameCamera_ = std::make_unique<GameCamera>();
	player_ = std::make_unique<Player>();
	enemyManager_ = std::make_unique<EnemyManager>();
	reticle_ = std::make_unique<Reticle>();
	mousePosView_.reset(Object3d::CreateModel("cube",".obj"));
////////////////////////////////////////////////////////////////////////////////////////////
//  初期化
////////////////////////////////////////////////////////////////////////////////////////////
	debugCamera_->Init();
	gameCamera_->Init();
	railManager_->Init();
	enemyManager_->Init();
	viewProjection_.Init();
	player_->Init();
	reticle_->Init();
	mousePosTransform_.Init();
////////////////////////////////////////////////////////////////////////////////////////////
//  セット
////////////////////////////////////////////////////////////////////////////////////////////
	player_->SetReticle(reticle_.get());
	reticle_->SetPlayer(player_.get());

	//parent
	player_->SetParent(&gameCamera_->GetWorldTransform());/// プレイヤーの親にゲームカメラ
	
}

void GameScene::Update() {
	//可視化オブジェクトに代入
	mousePosTransform_.translation_ = Input::GetMousePos3D(viewProjection_, mouseDepth_);
	Debug();//デバッグ
	//制御点追加
	if (Input::GetInstance()->IsTriggerMouse(3)) {
		railManager_->AddRail(mousePosTransform_.translation_);
	}else if (Input::GetInstance()->IsTriggerMouse(4)) {
		enemyManager_->AddEnemy(mousePosTransform_.translation_);
	}
	//カメラ更新
	gameCamera_->Update(railManager_->GetControlPoints());
	
	mousePosTransform_.UpdateMatrix();
	ViewProjectionUpdate();

////////////////////////////////////////////////////////////////////////////////////////////
//  各クラス更新
////////////////////////////////////////////////////////////////////////////////////////////

	//レールカメラ更新
	railManager_->Update();
	/// 敵マネージャー更新
	enemyManager_->Update();
	//プレイヤー更新
	player_->Update();
	///レティクル更新
	reticle_->Updata(viewProjection_);
}

void GameScene::Draw() {

	//Draw********************************************************
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);
	railManager_->Draw(viewProjection_);
	//プレイヤー
	if (isDebugCameraActive_) {
		player_->Draw(viewProjection_);
	}
	player_->BulletDraw(viewProjection_);
	////可視化オブジェクト
	mousePosView_->Draw(mousePosTransform_,viewProjection_);
	///敵
	enemyManager_->Draw(viewProjection_);
	//レール
	gameCamera_->RailDraw(viewProjection_);

#pragma endregion

#pragma region 3Dオブジェクトパーティクル描画
	Model::PreDrawParticle(commandList);

#pragma endregion
	Sprite::PreDraw(commandList);
	//レティクル
	reticle_->Draw();
	////スプライト描画

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
	ImGui::Text("3DX:%5.4f, 3DY:%5.4f, 3DZ:%5.4f", mousePosTransform_.translation_.x, mousePosTransform_.translation_.y, mousePosTransform_.translation_.z);
	ImGui::Text("2DX:%5.4f, 2DY:%5.4f", Input::GetMousePos().x, Input::GetMousePos().y);


	ImGui::End();
	//ライティング
	ImGui::Begin("Lighting");
	mousePosView_->DebugImgui();
	ImGui::End();
	gameCamera_->Debug();
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