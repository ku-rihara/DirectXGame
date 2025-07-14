#include "KTGame.h"
/// Scene
#include "Scene/Factory/SceneFactory.h"
#include"OffScreen/OffScreenRenderer.h"
//renderer
#include"base/SkyBoxRenderer.h"
//utility
#include "Collider/CollisionManager.h"

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

	pSceneManager_->ChangeScene("GAMEPLAY");
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
    /// SkyBox描画
    // --------------------------------------------------------------------------
    SkyBoxRenderer::GetInstance()->PreDraw(commandList);
    /// ゲームシーン描画
    pSceneManager_->SkyBoxDraw(); 
	// --------------------------------------------------------------------------
	/// モデル描画
	// --------------------------------------------------------------------------
	Model::PreDraw(commandList);
    /// ゲームシーン描画
	pSceneManager_->ModelDraw();
    /// コリジョン描画
	collisionManager_->Draw(pSceneManager_->GetScene()->GetViewProjection());	

	// --------------------------------------------------------------------------
	/// スプライト描画
	// --------------------------------------------------------------------------
	Sprite::PreDraw(commandList);

	/// ゲームシーン描画
	pSceneManager_->SpriteDraw();

}

void KTGame::DarwOffscreen() {

	/// commandList取得
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();

	OffScreenRenderer::GetInstance()->DrawImGui(); 
	OffScreenRenderer::GetInstance()->Draw(commandList);
	
}

// =============================================================
// 終了処理
// =============================================================
void KTGame::Finalize() {
	KTFramework::Finalize();
}
