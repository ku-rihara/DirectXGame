#include "GameScene.h"
#include "base/TextureManager.h"
//class
#include"utility/Particle/ParticleManager.h"

//math
#include"Frame/Frame.h"
#include"Scene/Manager/SceneManager.h"
#include"Lighrt/Light.h"

#include<imgui.h>


GameScene::GameScene() {}

GameScene::~GameScene() {

}

void GameScene::Init() {

	BaseScene::Init();
	
}

void GameScene::Update() {


	/// debugcamera
	debugCamera_->Update();
	Debug();

	
	
	
	ViewProjectionUpdate();


	if (input_->TrrigerKey(DIK_RETURN)) {
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}
}

/// ===================================================
/// モデル描画
/// ===================================================
void GameScene::ModelDraw() {

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
	Light::GetInstance()->DebugImGui();

#endif
}


// ビュープロジェクション更新
void GameScene::ViewProjectionUpdate() {
	BaseScene::ViewProjectionUpdate();
}


void GameScene::ViewProssess() {
	viewProjection_.UpdateMatrix();
}
