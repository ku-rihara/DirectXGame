#include "KTGame.h"
#include"utility/Particle/ParticleCommon.h"
/// Scene
#include "Scene/Factory/SceneFactory.h"


// ============================
// 初期化処理
// ============================
void KTGame::Init() {
	KTFramework::Init();

	// シーン
	sceneManager_ = SceneManager::GetInstance();
	sceneFactory_ = std::make_unique<SceneFactory>();
	sceneManager_->SetSceneFactory(sceneFactory_.get());
	sceneManager_->ChangeScene("TITLE");
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
	sceneManager_->ModelDraw();
	/// コリジョン描画
	collisionManager_->Draw(sceneManager_->GetScene()->GetViewProjection());
	///////////////////////////////////////////
	/// パーティクル
	///////////////////////////////////////////
	ParticleCommon::GetInstance()->PreDraw(commandList);
	/// ゲームシーン描画
	sceneManager_->ParticleDraw();
	///////////////////////////////////////////
	/// スプライト
    ///////////////////////////////////////////
	Sprite::PreDraw(commandList);
	sceneManager_->SpriteDraw();
}

// ============================
// 終了処理
// ============================
void KTGame::Finalize() {
	KTFramework::Finalize();
}
