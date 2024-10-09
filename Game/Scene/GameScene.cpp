#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <imgui.h>
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


	//ビュープロジェクション
	viewProjection_.Init();
	//emitter

}

void GameScene::Update() {

	debugCamera_->Update();
	Debug();//デバッグ
	// カメラ行列の計算をデバッグカメラのビュープロジェクションから行う
	viewProjection_.matView_ = debugCamera_->GetViewProjection().matView_;
	viewProjection_.matProjection_ = debugCamera_->GetViewProjection().matProjection_;
	viewProjection_.cameraMatrix_ = debugCamera_->GetViewProjection().cameraMatrix_;
	/*viewProjection_.UpdateMatrix();*/
}

void GameScene::Draw() {

	//Draw********************************************************
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	Model::PreDraw(commandList);

	Model::PreDrawParticle(commandList);


	Sprite::PreDraw(commandList);
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

	ImGui::End();
	//ライティング
	ImGui::Begin("Lighting");

	ImGui::End();

#endif
}