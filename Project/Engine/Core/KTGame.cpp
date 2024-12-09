#include "KTGame.h"
#include"utility/Particle/ParticleCommon.h"

// ============================
// 初期化処理
// ============================
void KTGame::Init() {
	KTFramework::Init();

	std::unique_ptr<BaseScene>scene = std::make_unique<BaseScene>();
	scemeManager_->SetNextScene(scene.get());
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
	scemeManager_->ModelDraw();
	/// コリジョン描画
	collisionManager_->Draw(scemeManager_->GetScene()->GetViewProjection());
	///////////////////////////////////////////
	/// パーティクル
	///////////////////////////////////////////
	ParticleCommon::GetInstance()->PreDraw(commandList);
	/// ゲームシーン描画
	scemeManager_->ParticleDraw();
	///////////////////////////////////////////
	/// スプライト
    ///////////////////////////////////////////
	Sprite::PreDraw(commandList);
	scemeManager_->SpriteDraw();
}

// ============================
// 終了処理
// ============================
void KTGame::Finalize() {
	KTFramework::Finalize();
}
