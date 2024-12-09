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
	
	// エミッター
	emitter_.reset(ParticleEmitter::CreateParticle("test", "Plane", ".obj", 1600,false));
	leftEmitter_.reset(ParticleEmitter::CreateParticle("LeftSide", "cube", ".obj", 300,false));
	rightEmitter_.reset(ParticleEmitter::CreateParticle("RightSide", "cube", ".obj", 300,false));

	
	//テクスチャハンドル
	circleHandle_= TextureManager::GetInstance()->LoadTexture("./Resources/circle.png");
	defaultHandle_ = TextureManager::GetInstance()->LoadTexture("./Resources/default.png");
	uv_ = TextureManager::GetInstance()->LoadTexture("Resources/uvChecker.png");

	ground_ = std::make_unique<Ground>();
	ground_->Init();

	emitter_->SetTextureHandle(circleHandle_);
	leftEmitter_->SetTextureHandle(defaultHandle_);
	rightEmitter_->SetTextureHandle(defaultHandle_);

	///=======================================================================================
	///Particle
	///=======================================================================================

	
	//WorldTransform
	//ビュープロジェクション
	viewProjection_.Init();

	viewProjection_.translation_ = { 0,-6.2f,-109.0f };

}

void GameScene::Update() {

	//emitter
	emitter_->EditorUpdate();
	emitter_->Emit();

	leftEmitter_->EditorUpdate();
	leftEmitter_->Emit();

	rightEmitter_->EditorUpdate();
	rightEmitter_->Emit();

	ParticleManager::GetInstance()->Update(&viewProjection_);

	/// debugcamera
	debugCamera_->Update();

	//各クラス更新
	ground_->Update();

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

	//平面描画
	if (isDraw) {
	
		ground_->Draw(viewProjection_);

		emitter_->DebugDraw(viewProjection_);
		leftEmitter_->DebugDraw(viewProjection_);
		rightEmitter_->DebugDraw(viewProjection_);
	}
}

/// ===================================================
   /// パーティクル描画
   /// ===================================================
void GameScene::ParticleDraw() {
	emitter_->RailDraw(viewProjection_);
	ParticleManager::GetInstance()->Draw(viewProjection_);
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