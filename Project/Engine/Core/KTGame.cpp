#include "KTGame.h"


// ============================
// 初期化処理
// ============================
void KTGame::Init() {
	KTFramework::Init();
}

// ============================
// 更新処理
// ============================
void KTGame::Update() {
	KTFramework::Update();
}

// ============================
// 描画処理
// ============================
void KTGame::Draw() {
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();
	///////////////////////////////////////////
	/// モデル
	///////////////////////////////////////////
	Model::PreDraw(commandList);
	/// ゲームシーン描画
	gameScene_->ModelDraw();
	/// コリジョン描画
	collisionManager_->Draw(gameScene_->GetViewProjection());
	///////////////////////////////////////////
	/// パーティクル
	///////////////////////////////////////////
	Model::PreDrawParticle(commandList);
	/// ゲームシーン描画
	gameScene_->ParticleDraw();
	///////////////////////////////////////////
	/// スプライト
    ///////////////////////////////////////////
	Sprite::PreDraw(commandList);
	gameScene_->SpriteDraw();
}

// ============================
// 終了処理
// ============================
void KTGame::Finalize() {
	KTFramework::Finalize();
}
