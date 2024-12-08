#include "GameScene.h"
#include "base/TextureManager.h"
//class
#include"3d/Light.h"
#include"utility/Particle/ParticleManager.h"

//math
#include"Frame/Frame.h"
#include<imgui.h>

GameScene::GameScene() {}

GameScene::~GameScene() {

}

void GameScene::Init() {

	// メンバ変数の初期化
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	textureManager_ = TextureManager::GetInstance();

	//デバッグカメラ
	debugCamera_ = std::make_unique<DebugCamera>(1280, 720);
	debugCamera_->Init();

	//音
	soundDataHandle_ = audio_->SoundLoadWave("Resources/fanfare.wav");
	//モデル

	modelPlane_.reset(Object3d::CreateModel("Plane", ".obj"));
	modelFence_.reset(Object3d::CreateModel("Fence", ".obj"));
	modelSuzanne_.reset(Object3d::CreateModel("Suzanne", ".obj"));
	modelSuzanne2_.reset(Object3d::CreateModel("Suzanne", ".obj"));
	modelTerrain_.reset(Object3d::CreateModel("terrain", ".obj"));

	ground_ = std::make_unique<Ground>();
	ground_->Init();

	/// test
	/*collisionTest1_ = std::make_unique<CollisionTest1>();
	collisionTest2_ = std::make_unique<CollisionTest2>();
	collisionTest1_->Init();
	collisionTest2_->Init();*/

	///トランスフォーム初期化
	planeTransform_.Init();
	fenceTransform_.Init();
	suzanneTransform_.Init();
	suzanneTransform2_.Init();
	terrainTransform_.Init();
	// エミッター
	emitter_.reset(ParticleEmitter::CreateParticle("test", "cube", ".obj", 300,false));
	leftEmitter_.reset(ParticleEmitter::CreateParticle("LeftSide", "cube", ".obj", 300,false));
	rightEmitter_.reset(ParticleEmitter::CreateParticle("RightSide", "cube", ".obj", 300,false));

	//テクスチャハンドル
	circleHandle_ = TextureManager::GetInstance()->LoadTexture("./Resources/default.png");
	uv_ = TextureManager::GetInstance()->LoadTexture("Resources/uvChecker.png");

	leftEmitter_->SetTextureHandle(circleHandle_);
	rightEmitter_->SetTextureHandle(circleHandle_);

	///=======================================================================================
	///Particle
	///=======================================================================================

	//スプライト生成
	sprite_ = std::make_unique<Sprite>();
	sprite_->CreateSprite(uv_, Vector2{ 0,0 }, Vector4(1, 1, 1, 1));

	sprite2_ = std::make_unique<Sprite>();
	sprite2_->CreateSprite(uv_, Vector2{ 700,0 }, Vector4(1, 1, 1, 1));
	//WorldTransform
	//ビュープロジェクション
	viewProjection_.Init();

	viewProjection_.translation_ = { 0,-6,-130.0f };

}

void GameScene::Update() {

	//emitter
	emitter_->EditorUpdate();
	emitter_->Emit();

	leftEmitter_->EditorUpdate();
	leftEmitter_->Emit();

	rightEmitter_->EditorUpdate();
	rightEmitter_->Emit();

	ParticleManager::GetInstance()->Update(&viewProjection_);

	/// debugcamera
	debugCamera_->Update();

	//各クラス更新
	ground_->Update();

	Debug();
	ViewProjectionUpdate();

	//if (Input::GetInstance()->TrrigerKey(DIK_F)) {
	//	Audio::GetInstance()->SoundPlayWave(soundDataHandle_);
	//}

	//Debug();//デバッグ

	////インプット処理
	//if (Input::GetInstance()->PushKey(DIK_LEFT)) {
	//	suzanneTransform_.translation_.x -= 0.01f;
	//}
	//if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
	//	suzanneTransform_.translation_.x += 0.01f;
	//}
	//if (Input::GetInstance()->PushKey(DIK_UP)) {
	//	suzanneTransform_.translation_.y += 0.01f;
	//}
	//if (Input::GetInstance()->PushKey(DIK_DOWN)) {
	//	suzanneTransform_.translation_.y -= 0.01f;
	//}


	/*collisionTest1_->Init();
	collisionTest2_->Init();*/

	//ワールド行列更新
	//	///トランスフォーム初期化
	//planeTransform_.UpdateMatrix();
	//fenceTransform_.UpdateMatrix();
	//suzanneTransform_.UpdateMatrix();
	//suzanneTransform2_.UpdateMatrix();
	//terrainTransform_.UpdateMatrix();
	/*modelPlaneParticle_->Update(&viewProjection_);*/


	// カメラ行列の計算をデバッグカメラのビュープロジェクションから行う
	/*viewProjection_.matView_ = debugCamera_->GetViewProjection().matView_;
	viewProjection_.matProjection_ = debugCamera_->GetViewProjection().matProjection_;
	viewProjection_.cameraMatrix_ = debugCamera_->GetViewProjection().cameraMatrix_;*/
	/*viewProjection_.UpdateMatrix();*/
}

/// ===================================================
/// モデル描画
/// ===================================================
void GameScene::ModelDraw() {

	//平面描画
	if (isDraw) {
		//modelSuzanne2_->color_.SetColor(Vector4(0, 0, 1, 1));
		///*modelPlane_->Draw(viewProjection_);
		//modelFence_->Draw(viewProjection_);*/
		//modelSuzanne_->Draw(suzanneTransform_, viewProjection_);
		//modelSuzanne2_->Draw(suzanneTransform2_, viewProjection_);
		//modelTerrain_->Draw(terrainTransform_, viewProjection_);

		//collisionTest1_->Draw();
		//collisionTest2_->Draw();

		ground_->Draw(viewProjection_);
	/*	emitter_->PositionDraw(viewProjection_);
		leftEmitter_->PositionDraw(viewProjection_);
		rightEmitter_->PositionDraw(viewProjection_);*/
	}
}

/// ===================================================
   /// パーティクル描画
   /// ===================================================
void GameScene::ParticleDraw() {
	emitter_->RailDraw(viewProjection_);
	ParticleManager::GetInstance()->Draw(viewProjection_);
}

/// ===================================================
   /// スプライト描画
   /// ===================================================
void GameScene::SpriteDraw() {
	//////スプライト描画
	//sprite_->Draw();
	//sprite2_->Draw();
}

void GameScene::Debug() {
#ifdef _DEBUG
	ImGui::Begin("Camera");
	ImGui::DragFloat3("pos", &viewProjection_.translation_.x, 0.1f);
	ImGui::DragFloat3("rotate", &viewProjection_.rotation_.x, 0.1f);
	ImGui::End();


#endif
}


// ビュープロジェクション更新
void GameScene::ViewProjectionUpdate() {

#ifdef _DEBUG
	// デバッグカメラモード切り替え------------------------------
	if (Input::GetInstance()->TrrigerKey(DIK_SPACE)) {
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
		viewProjection_.UpdateMatrix();

		/*viewProjection_.TransferMatrix();*/
	}
}