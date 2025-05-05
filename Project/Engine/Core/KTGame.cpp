#include "KTGame.h"
#include"utility/ParticleEditor/ParticleCommon.h"
/// Scene
#include "Scene/Factory/SceneFactory.h"
#include"base/FullscreenRenderer.h"

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
	pSceneManager_->ChangeScene("GAMEPLAY");
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
	/// スプライト描画
	// --------------------------------------------------------------------------
	Sprite::PreDraw(commandList);

	pSceneManager_->SpriteDraw();/// ゲームシーン描画

}

void KTGame::DarwOffscreen() {

	/// commandList取得
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();

	FullScreenRenderer::GetInstance()->DrawImGui(); 
	FullScreenRenderer::GetInstance()->Draw(commandList);/// 画像コピー描画
	
}

// =============================================================
// 終了処理
// =============================================================
void KTGame::Finalize() {
	KTFramework::Finalize();
}
