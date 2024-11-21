#include "GameScene.h"
#include "base/TextureManager.h"
#include <cassert>
//class
#include"3d/Light.h"
//math
#include"MinMax.h"
#include"Frame/Frame.h"

GameScene::GameScene() {}

GameScene::~GameScene() {

}

void GameScene::Init() {
	std::random_device seedGenerator;
	std::mt19937 randomEngine(seedGenerator());
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
	std::uniform_real_distribution<float>lifeTimedist(1.0f, 3.0f);
	modelPlane_.reset(Object3d::CreateModel("Plane", ".obj"));
	modelPlaneParticle_.reset(Object3dParticle::CreateModel("Plane", ".obj", modelInstanceMax_));
	modelFence_.reset(Object3d::CreateModel("Fence", ".obj"));
	modelSuzanne_.reset(Object3d::CreateModel("Suzanne", ".obj"));
	modelSuzanne2_.reset(Object3d::CreateModel("Suzanne", ".obj"));
	modelTerrain_.reset(Object3d::CreateModel("terrain", ".obj"));

	///トランスフォーム初期化
	planeTransform_.Init();
	fenceTransform_.Init();
	suzanneTransform_.Init();
	suzanneTransform2_.Init();
	terrainTransform_.Init();

	////テクスチャハンドル
	uvHandle_ = TextureManager::GetInstance()->LoadTexture("./Resources/circle.png");
	uint32_t uv_ = TextureManager::GetInstance()->LoadTexture("./Resources/uvChecker.png");

	//スプライト生成
	sprite_ = std::make_unique<Sprite>();
	sprite_->CreateSprite(uv_, Vector2{ 0,0 }, Vector4(1, 1, 1, 1));

	sprite2_ = std::make_unique<Sprite>();
	sprite2_->CreateSprite(uv_, Vector2{ 700,0 }, Vector4(1, 1, 1, 1));
	//WorldTransform
	//ビュープロジェクション
	viewProjection_.Init();
	///=======================================================================================
	///Particle
	///=======================================================================================
		//emitter
	modelPlaneParticle_->emitter_.count = 3;
	modelPlaneParticle_->emitter_.frequency = 0.5f;
	modelPlaneParticle_->emitter_.frequencyTime = 0.0f;
	//acceleration
	modelPlaneParticle_->accelerationField_.acceleration = { 15.0f,0.0f,0.0f };
	modelPlaneParticle_->accelerationField_.area.min = { -1.0f,-1.0f,-1.0f };
	modelPlaneParticle_->accelerationField_.area.max = { 1.0f,1.0f,1.0f };

}

void GameScene::Update() {


	debugCamera_->Update();

	if (Input::GetInstance()->TrrigerKey(DIK_F)) {
		Audio::GetInstance()->SoundPlayWave(soundDataHandle_);
	}

	Debug();//デバッグ

	//インプット処理
	if (Input::GetInstance()->PushKey(DIK_LEFT)) {
		suzanneTransform_.translation_.x -= 0.01f;
	}
	if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
		suzanneTransform_.translation_.x += 0.01f;
	}
	if (Input::GetInstance()->PushKey(DIK_UP)) {
		suzanneTransform_.translation_.y += 0.01f;
	}
	if (Input::GetInstance()->PushKey(DIK_DOWN)) {
		suzanneTransform_.translation_.y -= 0.01f;
	}

	modelPlaneParticle_->emitter_.frequencyTime += Frame::DeltaTime();//時刻すすめる
	//頻度より大きいなら
	if (modelPlaneParticle_->emitter_.frequency <= modelPlaneParticle_->emitter_.frequencyTime) {
		modelPlaneParticle_->Emit(modelPlaneParticle_->emitter_, MinMax(-1.0f, 1.0f), MinMax(0.0f, 1.0f), 5);

		modelPlaneParticle_->emitter_.frequencyTime -= modelPlaneParticle_->emitter_.frequency ;//時刻すすめる
	}


	//ワールド行列更新
		///トランスフォーム初期化
	planeTransform_.UpdateMatrix();
	fenceTransform_.UpdateMatrix();
	suzanneTransform_.UpdateMatrix();
	suzanneTransform2_.UpdateMatrix();
	terrainTransform_.UpdateMatrix();
	modelPlaneParticle_->Update(&viewProjection_);


	// カメラ行列の計算をデバッグカメラのビュープロジェクションから行う
	viewProjection_.matView_ = debugCamera_->GetViewProjection().matView_;
	viewProjection_.matProjection_ = debugCamera_->GetViewProjection().matProjection_;
	viewProjection_.cameraMatrix_ = debugCamera_->GetViewProjection().cameraMatrix_;
	/*viewProjection_.UpdateMatrix();*/
}

    /// ===================================================
	/// モデル描画
	/// ===================================================
void GameScene::ModelDraw() {

	//平面描画
	if (isDraw) {
		modelSuzanne2_->color_.SetColor(Vector4(0, 0, 1, 1));
		/*modelPlane_->Draw(viewProjection_);
		modelFence_->Draw(viewProjection_);*/
		modelSuzanne_->Draw(suzanneTransform_, viewProjection_);
		modelSuzanne2_->Draw(suzanneTransform2_, viewProjection_);
		modelTerrain_->Draw(terrainTransform_, viewProjection_);
	}
}

/// ===================================================
   /// パーティクル描画
   /// ===================================================
void GameScene::ParticleDraw() {
	modelPlaneParticle_->Draw(viewProjection_, uvHandle_);
}

/// ===================================================
   /// スプライト描画
   /// ===================================================
void GameScene::SpriteDraw() {
	////スプライト描画
	sprite_->Draw();
	sprite2_->Draw();
}



void GameScene::Debug() {
#ifdef _DEBUG
	ImGui::Begin("Window");

	if (ImGui::TreeNode("Frag")) {
		ImGui::Checkbox("isDraw", &isDraw);
		ImGui::Checkbox("isAcceleration", &modelPlaneParticle_->accelerationField_.isAdaption);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("ViewProjection")) {

		ImGui::DragFloat3("Scale", &viewProjection_.scale_.x, 0.01f);
		ImGui::DragFloat3("Rotate", &viewProjection_.rotation_.x, 0.01f);
		ImGui::DragFloat3("Translate", &viewProjection_.translation_.x, 0.01f);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("WorldTransform")) {

		if (ImGui::TreeNode("Plane")) {
			ImGui::DragFloat3("Scale", &planeTransform_.scale_.x, 0.01f);
			ImGui::DragFloat3("Rotate", &planeTransform_.rotation_.x, 0.01f);
			ImGui::DragFloat3("Translate", &planeTransform_.translation_.x, 0.01f);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Fence")) {
			ImGui::DragFloat3("Scale", &fenceTransform_.scale_.x, 0.01f);
			ImGui::DragFloat3("Rotate", &fenceTransform_.rotation_.x, 0.01f);
			ImGui::DragFloat3("Translate", &fenceTransform_.translation_.x, 0.01f);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Suzanne")) {
			ImGui::DragFloat3("Scale", &suzanneTransform_.scale_.x, 0.01f);
			ImGui::DragFloat3("Rotate", &suzanneTransform_.rotation_.x, 0.01f);
			ImGui::DragFloat3("Translate", &suzanneTransform_.translation_.x, 0.01f);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Sprite")) {
			ImGui::DragFloat3("Scale", &sprite_->transform_.scale.x, 0.1f);
			ImGui::DragFloat3("Rotate", &sprite_->transform_.rotate.x, 0.1f);
			ImGui::DragFloat3("Translate", &sprite_->transform_.translate.x, 1.0f);
			/*ImGui::DragFloat3("Translate", &sprite_->transform_.translate.x, 1.0f);*/
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Particle")) {
		ImGui::DragFloat3("Scale", &modelPlaneParticle_->emitter_.transform.scale.x, 0.01f);
		ImGui::DragFloat3("Rotate", &modelPlaneParticle_->emitter_.transform.rotate.x, 0.01f);
		ImGui::DragFloat3("Translate", &modelPlaneParticle_->emitter_.transform.translate.x, 0.01f);
		ImGui::TreePop();
	}

	ImGui::End();
	//ライティング
	ImGui::Begin("Lighting");
	Light::GetInstance()->DebugImGui();
	if (ImGui::TreeNode("Plane")) {
		modelPlane_->DebugImgui();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Suzanne")) {
		modelSuzanne_->DebugImgui();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Terrian")) {
		modelTerrain_->DebugImgui();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Sprite")) {
		sprite_->DebugImGui();
		ImGui::TreePop();
	}
	ImGui::End();

#endif
}