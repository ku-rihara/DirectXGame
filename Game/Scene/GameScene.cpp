#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>


GameScene::GameScene() {}

GameScene::~GameScene() {

}

void GameScene::Init() {
	
	// メンバ変数の初期化
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	textureManager_ = TextureManager::GetInstance();

	soundDataHandle_ = audio_->SoundLoadWave("Resources/fanfare.wav");
	modelPlane_ = std::unique_ptr<Model>(Model::Create("Plane"));
	modelFence_ = std::unique_ptr<Model>(Model::Create("Fence"));
	modelInstance_ = modelPlane_->GetKnumInstance();

	sprite_ = std::make_unique<Sprite>();
	sprite_->CreateSprite();

	planeTransforms_.reserve(modelInstance_);
	for (size_t i = 0; i < modelInstance_; ++i) {
		planeTransforms_.emplace_back(std::make_unique<WorldTransform>());
	}
	debugCamera_ = std::make_unique<DebugCamera>(1280, 720);
	debugCamera_->Init();

	// ワールドトランスフォーム初期化
	planeTransform_.Init();
	fenceTransform_.Init();
	transformSprite_.Init();
	uvTransformSprite_.Init();

	// PlaneParticle
	for (uint32_t index = 0; index < modelInstance_; ++index) {
		planeTransforms_[index]->Init();
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
		if (ImGui::TreeNode("Fence")) {
			ImGui::DragFloat3("Scale", &fenceTransform_.scale_.x, 0.01f);
			ImGui::DragFloat3("Rotate", &fenceTransform_.rotation_.x, 0.01f);
			ImGui::DragFloat3("Translate", &fenceTransform_.translation_.x, 0.01f);
			ImGui::TreePop();
		}
		

		if (ImGui::TreeNode("Sprite")) {
			ImGui::DragFloat3("Scale", &transformSprite_.scale_.x, 0.1f);
			ImGui::DragFloat3("Rotate", &transformSprite_.rotation_.x, 1.0f);
			ImGui::DragFloat3("Translate", &transformSprite_.translation_.x, 1.0f);
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
	if (ImGui::TreeNode("Plane")) {
		modelPlane_->DebugImGui();
		ImGui::TreePop();
	}
	ImGui::End();

#endif
	//ワールド行列更新
	planeTransform_.UpdateMatrix();
	fenceTransform_.UpdateMatrix();

	for (uint32_t i = 0; i < modelInstance_; i++) {
		planeTransforms_[i]->UpdateMatrix();
	}

	transformSprite_.UpdateMatrix();
	uvTransformSprite_.UpdateMatrix();

	//tramsform.rotate.y += 0.03f;

	// カメラ行列の計算をデバッグカメラのビュープロジェクションから行う
	viewProjection_.matView_ = debugCamera_->GetViewProjection().matView_;
	viewProjection_.matProjection_ = debugCamera_->GetViewProjection().matProjection_;

	//スプライト
	Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kWindowWidth), float(WinApp::kWindowHeight), 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrixSprite = transformSprite_.matWorld_ * projectionMatrixSprite;
	sprite_->SetTransformationMatrixDataSprite(worldViewProjectionMatrixSprite);

	//UVTransform
	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransformSprite_.scale_);
	uvTransformMatrix = (uvTransformMatrix * MakeRotateZMatrix(uvTransformSprite_.rotation_.z));
	uvTransformMatrix = (uvTransformMatrix * MakeTranslateMatrix(uvTransformSprite_.translation_));
	sprite_->SetUVTransformSprite(uvTransformMatrix);

}

void GameScene::Draw() {

	//Draw********************************************************

		//平面描画
	if (isDrawPlane_) {

		/*	modelPlane->DrawParticle(PlaneTransforms, viewProjection);*/

		modelPlane_->Draw(planeTransform_, viewProjection_);
		modelFence_->Draw(fenceTransform_, viewProjection_);
		///*modelSuzanne_->Draw(SuzanneTransform, viewProjection);


		////スプライト描画
		//sprite->DrawSprite(textureManager->GetTextureHandle(uvHandle));
	}
}
