#include "KTGame.h"
#include"utility/Particle/ParticleCommon.h"
/// Scene
#include "Scene/Factory/SceneFactory.h"


// =============================================================
// 初期化処理
// =============================================================
void KTGame::Init() {
	KTFramework::Init();

	// シーン工場生成
	sceneFactory_ = std::make_unique<SceneFactory>();

	// タイトルから始める
	pSceneManager_ = SceneManager::GetInstance();
	pSceneManager_->SetSceneFactory(sceneFactory_.get());
	pSceneManager_->ChangeScene("TITLE");
}

// =============================================================
// 更新処理
// =============================================================
void KTGame::Update() {
	KTFramework::Update();
}

// =============================================================
// 描画処理
// =============================================================
void KTGame::Draw() {

	/// commandList取得
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();

	// --------------------------------------------------------------------------
	/// モデル描画
	// --------------------------------------------------------------------------
	Model::PreDraw(commandList);
	
	pSceneManager_->ModelDraw();/// ゲームシーン描画

	collisionManager_->Draw(pSceneManager_->GetScene()->GetViewProjection());	/// コリジョン描画

	// --------------------------------------------------------------------------
	/// パーティクル描画
	// --------------------------------------------------------------------------
	ParticleCommon::GetInstance()->PreDraw(commandList);
	
	pSceneManager_->ParticleDraw();/// ゲームシーン描画

	// --------------------------------------------------------------------------
	/// スプライト描画
	// --------------------------------------------------------------------------
	Sprite::PreDraw(commandList);

	pSceneManager_->SpriteDraw();/// ゲームシーン描画
}

// =============================================================
// 終了処理
// =============================================================
void KTGame::Finalize() {
	KTFramework::Finalize();
}
