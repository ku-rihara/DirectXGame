#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
//class
#include"Light.h"


GameScene::GameScene() {}

GameScene::~GameScene() {

}

void GameScene::Init() {
	
	// メンバ変数の初期化
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	textureManager_ = TextureManager::GetInstance();
	
	//デバッグカメラ
	debugCamera_ = std::make_unique<DebugCamera>(1280, 720);
	debugCamera_->Init();

	//音
	soundDataHandle_ = audio_->SoundLoadWave("Resources/fanfare.wav");
	//モデル
	modelPlane_.reset(Model::Create("Plane"));
	modelPlaneParticle_.reset(Model::CreateParticle("Plane"));
	modelFence_.reset(Model::Create("Fence"));
	modelSuzanne_.reset(Model::Create("Suzanne"));
	modelTerrain_.reset(Model::Create("terrain"));
	
	modelInstance_ = modelPlaneParticle_->GetKnumInstance();

	////テクスチャハンドル
	uvHandle_ = TextureManager::GetInstance()->LoadTexture("./Resources/UVChecker.png");
		
	//スプライト生成
	sprite_ = std::make_unique<Sprite>();
	sprite_->CreateSprite(uvHandle_,Vector2{0,0},Vector4(1,1,1,1));
	//WorldTransform

	planeTransforms_.reserve(modelInstance_);
	for (uint32_t i = 0; i < modelInstance_; ++i) {
		planeTransforms_.emplace_back(std::make_unique<WorldTransform>());
	}

	// ワールドトランスフォーム初期化
	planeTransform_.Init();
	fenceTransform_.Init();
	suzanneTransform_.Init();
	terrainTransform_.Init();
	transformSprite_.Init();
	uvTransformSprite_.Init();

	// PlaneParticle
	for (uint32_t index = 0; index < modelInstance_; ++index) {
		planeTransforms_[index]->Init();
		planeTransforms_[index]->rotation_.y = -3;
		planeTransforms_[index]->translation_ = { index * 0.1f, index * 0.1f, index * 0.1f };
	}

	// ワールドトランスフォーム値セット
	transformSprite_.scale_.x = 0.7f;
	planeTransform_.rotation_.y = -3.0f;
}

void GameScene::Update() {
	debugCamera_->Update();

	if (Input::GetInstance()->TrrigerKey(DIK_F)) {
		Audio::GetInstance()->SoundPlayWave(soundDataHandle_);
	}
#ifdef _DEBUG
	ImGui::Begin("Window");

	if (ImGui::TreeNode("IsDrawModel")) {
		ImGui::Checkbox("isPlane", &isDrawPlane_);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("WorldTransform")) {

		if (ImGui::TreeNode("Plane")) {
			ImGui::DragFloat3("Scale", &planeTransform_.scale_.x, 0.01f);
			ImGui::DragFloat3("Rotate", &planeTransform_.rotation_.x, 0.01f);
			ImGui::DragFloat3("Translate", &planeTransform_.translation_.x, 0.01f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("PlaneParticle")) {
			ImGui::DragFloat3("Scale", &planeTransforms_[0]->scale_.x, 0.01f);
			ImGui::DragFloat3("Rotate", &planeTransforms_[0]->rotation_.x, 0.01f);
			ImGui::DragFloat3("Translate", &planeTransforms_[0]->translation_.x, 0.01f);
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
			ImGui::DragFloat3("Rotate", &sprite_->transform_.rotate.x, 1.0f);
			ImGui::DragFloat3("Translate", &sprite_->transform_.translate.x, 1.0f);
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("UVTransform")) {
		ImGui::DragFloat2("Scale", &uvTransformSprite_.scale_.x, 0.1f, -10.0f, 10.0f);
		ImGui::DragFloat2("Translate", &uvTransformSprite_.translation_.x, 0.01f, -10.0f, 10.0f);
		ImGui::SliderAngle("Rotate", &uvTransformSprite_.rotation_.z);
		ImGui::TreePop();
	}
	ImGui::End();
	//ライティング
	ImGui::Begin("Lighting");
	Light::GetInstance()->DebugImGui();
	if (ImGui::TreeNode("Plane")) {
		modelPlane_->DebugImGui();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Suzanne")) {
		modelSuzanne_->DebugImGui();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Terrian")) {
		modelTerrain_->DebugImGui();
		ImGui::TreePop();
	}
	ImGui::End();

#endif

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

	//ワールド行列更新
	planeTransform_.UpdateMatrix();
	fenceTransform_.UpdateMatrix();
	suzanneTransform_.UpdateMatrix();
	terrainTransform_.UpdateMatrix();

	for (uint32_t i = 0; i < modelInstance_; i++) {
		planeTransforms_[i]->UpdateMatrix();
	}

	transformSprite_.UpdateMatrix();
	uvTransformSprite_.UpdateMatrix();

	//tramsform.rotate.y += 0.03f;

	// カメラ行列の計算をデバッグカメラのビュープロジェクションから行う
	viewProjection_.matView_ = debugCamera_->GetViewProjection().matView_;
	viewProjection_.matProjection_ = debugCamera_->GetViewProjection().matProjection_;



}

void GameScene::Draw() {
	
	//Draw********************************************************
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
	modelPlaneParticle_->DrawParticle(planeTransforms_, viewProjection_);

	Model::PreDraw(commandList);
		//平面描画
	if (isDrawPlane_) {

		/*modelPlane_->Draw(planeTransform_, viewProjection_);
		modelFence_->Draw(fenceTransform_, viewProjection_);
		modelSuzanne_->Draw(suzanneTransform_, viewProjection_);
		modelTerrain_->Draw(terrainTransform_, viewProjection_);*/
	
		Sprite::PreDraw(commandList);
		////スプライト描画
		//sprite_->Draw();
	}
}
