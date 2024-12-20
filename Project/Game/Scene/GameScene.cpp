#include "GameScene.h"
#include<imgui.h>
#include"Scene/Manager/SceneManager.h"


GameScene::GameScene() {}

GameScene::~GameScene() {

}

void GameScene::Init() {

	BaseScene::Init();
	
	inputHandler_ = std::make_unique<InputHandler>();

	// assign command
	inputHandler_->AssignMoveRightCommand2PressKeyD();
	inputHandler_->AssignMoveLeftCommand2PressKeyA();

	player_ = std::make_unique<Player>();
	player_->Init();
}

void GameScene::Update() {


	/// debugcamera
	debugCamera_->Update();
	Debug();

	//各クラス更新
	iCommand_ = inputHandler_->HandleInput();

	if (this->iCommand_) {
		iCommand_->Exec(*player_);
	}

	player_->Update();

	
	ViewProjectionUpdate();


	if (input_->TrrigerKey(DIK_RETURN)) {
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}
}

/// ===================================================
/// モデル描画
/// ===================================================
void GameScene::ModelDraw() {
	player_->Draw(viewProjection_);

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
