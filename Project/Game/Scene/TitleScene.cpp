#include "TitleScene.h"
#include "base/TextureManager.h"
//class

//math
#include"Frame/Frame.h"
#include<imgui.h>

TitleScene::TitleScene() {}

TitleScene::~TitleScene() {

}

void TitleScene::Init() {

	// メンバ変数の初期化
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	textureManager_ = TextureManager::GetInstance();

	//デバッグカメラ
	debugCamera_ = std::make_unique<DebugCamera>(1280, 720);
	debugCamera_->Init();

	
	//ビュープロジェクション
	viewProjection_.Init();

	viewProjection_.translation_ = { 0,-0.2f,-0 };

}

void TitleScene::Update() {

	Debug();
	ViewProjectionUpdate();

}

/// ===================================================
/// モデル描画
/// ===================================================
void TitleScene::ModelDraw() {

	
}

/// ===================================================
   /// パーティクル描画
   /// ===================================================
void TitleScene::ParticleDraw() {
	
}

/// ===================================================
   /// スプライト描画
   /// ===================================================
void TitleScene::SpriteDraw() {

}

void TitleScene::Debug() {
#ifdef _DEBUG
	ImGui::Begin("Camera");
	ImGui::DragFloat3("pos", &viewProjection_.translation_.x, 0.1f);
	ImGui::DragFloat3("rotate", &viewProjection_.rotation_.x, 0.1f);
	ImGui::End();
#endif
}


// ビュープロジェクション更新
void TitleScene::ViewProjectionUpdate() {
	BaseScene::ViewProjectionUpdate();
}