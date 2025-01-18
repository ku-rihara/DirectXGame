#include "GameScene.h"
#include "base/TextureManager.h"
//class

#include"utility/Particle/ParticleManager.h"

//math
#include"Frame/Frame.h"
#include<imgui.h>

#include"Scene/Manager/SceneManager.h"


GameScene::GameScene() {}

GameScene::~GameScene() {

}

void GameScene::Init() {

	BaseScene::Init();
	

	ground_ = std::make_unique<Ground>();
	ground_->Init();

	

}

void GameScene::Update() {


	/// debugcamera
	debugCamera_->Update();
	Debug();

	//各クラス更新
	ground_->Update();

	
	ViewProjectionUpdate();


	if (input_->TrrigerKey(DIK_RETURN)) {
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}
}

/// ===================================================
/// モデル描画
/// ===================================================
void GameScene::ModelDraw() {
	ground_->Draw(viewProjection_);

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


void GameScene::ViewProssess() {
	viewProjection_.UpdateMatrix();
}
