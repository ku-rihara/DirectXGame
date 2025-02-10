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
	

	ground_ = std::make_unique<Ground>();
	monsterBall_= std::make_unique<MonsterBall>();
	plane_= std::make_unique<Plane>();

	monsterBall_->Init();
	ground_->Init();
	plane_->Init();

	isDebugCameraActive_ = true;
	

}

void GameScene::Update() {

	/// debugcamera
	debugCamera_->Update();
	Debug();

	//各クラス更新
	ground_->Update();
	monsterBall_->Update();
	plane_->Update();
	
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
	plane_->Draw(viewProjection_);
	monsterBall_->Draw(viewProjection_);
}

/// ======================================================
/// パーティクル描画
/// ======================================================
void GameScene::ParticleDraw() {

}

/// ======================================================
/// スプライト描画
/// ======================================================
void GameScene::SpriteDraw() {

}

void GameScene::Debug() {
#ifdef _DEBUG
	
	Light::GetInstance()->DebugImGui();
	ImGui::Begin("Object");
	ground_->Debug();
	monsterBall_->Debug();
	plane_->Debug();
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
