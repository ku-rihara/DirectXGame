#include "GameScene.h"
#include"Scene/Manager/SceneManager.h"
#include"utility/Particle/ParticleManager.h"
// base
#include "base/TextureManager.h"
//math
#include<imgui.h>

GameScene::GameScene() {}

GameScene::~GameScene() {

}

void GameScene::Init() {

	////* TODO 
	/// ライト使いやすく
	/// 2Dゲームをリファレンスとしたコンボを実装(来週まで)
	/// 敵の配置パターンのエディター(来週まで)


	// メンバ変数の初期化
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	textureManager_ = TextureManager::GetInstance();

	//デバッグカメラ
	debugCamera_ = std::make_unique<DebugCamera>(1280, 720);
	debugCamera_->Init();

	///=======================================================================================
	/// 生成
	///=======================================================================================

	field_ = std::make_unique<Field>();
	lockOn_ = std::make_unique<LockOn>();
	player_ = std::make_unique<Player>();
	gamecamera_ = std::make_unique<GameCamera>();
	enemyManager_ = std::make_unique<EnemyManager>();
	skydome_= std::make_unique<Skydome>();

	///=======================================================================================
	/// 初期化
	///=======================================================================================
	field_->Init();
	skydome_->Init();
	player_->Init();
	lockOn_->Init();
	enemyManager_->Init();
	gamecamera_->Init();


	viewProjection_.Init();//ビュープロジェクション


	gamecamera_->SetTarget(&player_->GetTransform());
	player_->SetViewProjection(&viewProjection_);
	player_->SetLockOn(lockOn_.get());


}

void GameScene::Update() {


	/// debugcamera
	debugCamera_->Update();
	Debug();

	//各クラス更新
	player_->Update();
	skydome_->Update();
	field_->Update();
	enemyManager_->Update();
	gamecamera_->Update();
	lockOn_->Update(enemyManager_->GetEnemies(), viewProjection_);

	/// パーティクル更新
	ParticleManager::GetInstance()->Update(&viewProjection_);
	
	ViewProjectionUpdate();

	/// タイトルに戻る
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
		skydome_->Draw(viewProjection_);
		field_->Draw(viewProjection_);
		enemyManager_->Draw(viewProjection_);
		player_->Draw(viewProjection_);
	}
}

/// ===================================================
   /// パーティクル描画
   /// ===================================================
void GameScene::ParticleDraw() {
	ParticleManager::GetInstance()->Draw(viewProjection_);
}

/// ===================================================
   /// スプライト描画
   /// ===================================================
void GameScene::SpriteDraw() {
	lockOn_->Draw();
}

void GameScene::Debug() {
#ifdef _DEBUG
	ImGui::Begin("Camera");
	ImGui::DragFloat3("pos", &viewProjection_.translation_.x, 0.1f);
	ImGui::DragFloat3("rotate", &viewProjection_.rotation_.x, 0.1f);
	ImGui::End();

	enemyManager_->ImGuiUpdate();


	ImGui::Begin("ParamaterEditor");
	player_->AdjustParm();
	ImGui::End();
#endif
}


// ビュープロジェクション更新
void GameScene::ViewProjectionUpdate() {
	BaseScene::ViewProjectionUpdate();
}

void GameScene::ViewProssess() {
	viewProjection_.matView_ = gamecamera_->GetViewProjection().matView_;
	viewProjection_.matProjection_ = gamecamera_->GetViewProjection().matProjection_;

	viewProjection_.TransferMatrix();

}