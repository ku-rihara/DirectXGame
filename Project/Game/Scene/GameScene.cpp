#include "GameScene.h"
#include "base/TextureManager.h"
//class
#include"3d/Light.h"
#include"utility/Particle/ParticleManager.h"

//math
#include"Frame/Frame.h"
#include<imgui.h>

#include"Scene/Manager/SceneManager.h"


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
	
	///=======================================================================================
	/// 生成
	///=======================================================================================

	ground_ = std::make_unique<Ground>();

	///=======================================================================================
	/// 初期化
	///=======================================================================================
	ground_->Init();

	//ビュープロジェクション
	viewProjection_.Init();
	viewProjection_.translation_ = { 0,-6.2f,-109.0f };
}

void GameScene::Update() {


	/// debugcamera
	debugCamera_->Update();

	//各クラス更新
	ground_->Update();

	Debug();
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
	
		ground_->Draw(viewProjection_);

	}
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
	BaseScene::ViewProjectionUpdate();
}