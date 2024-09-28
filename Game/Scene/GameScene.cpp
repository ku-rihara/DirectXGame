#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
//class
#include"Light.h"

std::unique_ptr<Model> GameScene::modelEffect_ = nullptr;
GameScene::GameScene() {}

GameScene::~GameScene() {

}

void GameScene::Init() {
	
	// メンバ変数の初期化
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	textureManager_ = TextureManager::GetInstance();
	
	//デバッグカメラ
	debugCamera_ = std::make_unique<DebugCamera>(1280, 720);
	debugCamera_->Init();

	//音
	soundDataHandle_ = audio_->SoundLoadWave("Resources/fanfare.wav");
	//モデル
	
	modelGround_.reset(Model::Create("Ground"));
	modelSkyDome_.reset(Model::Create("skydome"));
	modelFighterBody_.reset(Model::Create("PlayerBody"));
	modelFighterHead_.reset(Model::Create("PlayerHead"));
	modelFighterLeftArm_.reset(Model::Create("PlayerLeftArm"));
	modelFighterRightArm_.reset(Model::Create("PlayerRightArm"));
	modelPlayerWeapon_.reset(Model::Create("Hunmer"));
	modelEnemyBody_.reset(Model::Create("EnemyHead"));
	modelEnemyThurn_.reset(Model::Create("EnemyThurn"));
	GameScene::modelEffect_.reset(Model::Create("DamageEffect"));

	/// <summary>
	/// 生成
	/// </summary>
	player_ = std::make_unique<Player>();
	/*effect_ = std::make_unique<Effect>();*/
	hummer_ = std::make_unique<Hummer>();
	enemy_ = std::make_unique<Enemy>();
	skyDome_ = std::make_unique<Skydome>();
	ground_ = std::make_unique<Ground>();
	followCamera_ = std::make_unique<FollowCamera>();
	lockOn_ = std::make_unique<LockOn>();
	collisionManager_ = std::make_unique<CollisionManager>();
	// 武器初期化
	Model* weaponModel = modelPlayerWeapon_.get();
	hummer_->Init(weaponModel);
	player_->SetHummer(hummer_.get());

	//
	// 自キャラ初期化***********************************************************************
	std::vector<Model*> playerModels = { modelFighterBody_.get(), modelFighterHead_.get(), modelFighterLeftArm_.get(), modelFighterRightArm_.get() };
	player_->Init(playerModels);
	player_->SetLockOn(lockOn_.get());
	skyDome_->Init(modelSkyDome_.get());
	ground_->Init(modelGround_.get());

	// 追尾カメラ初期化*********************************************************************************
	followCamera_->Init();
	followCamera_->SetLockOn(lockOn_.get());
	// 敵キャラ初期化*******************************************************************************
	AddEnemy();
	worldTransform_.Init();
	viewProjection_.Init();
	// ロックオン初期化*****************************************************************************************
	lockOn_->Init();
	// コリジョンマネージャー
	collisionManager_->Init();
	// 自キャラのワールドトランスフォームを追従カメラにセット
	followCamera_->SetTarget(&player_->GetWorldTransform());
	player_->SetViewProjection(&followCamera_->GetViewProjection());


	////テクスチャハンドル
	/*uvHandle_ = TextureManager::GetInstance()->LoadTexture("./Resources/UVChecker.png");*/
		
	//スプライト生成
	sprite_ = std::make_unique<Sprite>();
	sprite_->CreateSprite();
	//WorldTransform

	planeTransforms_.reserve(modelInstance_);
	for (size_t i = 0; i < modelInstance_; ++i) {
		planeTransforms_.emplace_back(std::make_unique<WorldTransform>());
	}

	// ワールドトランスフォーム初期化
	

	// PlaneParticle
	for (uint32_t index = 0; index < modelInstance_; ++index) {
		planeTransforms_[index]->Init();
		planeTransforms_[index]->translation_ = { index * 0.1f, index * 0.1f, index * 0.1f };
	}

	// ワールドトランスフォーム値セット
	
}

void GameScene::Update() {
#ifdef _DEBUG
	// デバッグカメラモード切り替え------------------------------
	if (input_->TrrigerKey(DIK_SPACE)) {
		if (isDebugCameraActive_ == false) {
			isDebugCameraActive_ = true;
		}
		else if (isDebugCameraActive_ == true) {
			isDebugCameraActive_ = false;
		}
	}
	// デバッグカメラモード切り替え------------------------------
	collisionManager_->Update();
#endif
	// 各クラス更新処理---------
	skyDome_->Update();
	ground_->Update();
	player_->Update();
	// hummer_->Update();
	followCamera_->Update();

	for (std::unique_ptr<Enemy>& enemy : enemies_) {
		enemy->Update();
	}
	lockOn_->Update(enemies_, viewProjection_);

	// 衝突判定と応答
	CheckAllCollisions();
	// 衝突マネージャー更新(WorldTransform)
	collisionManager_->UpdateWorldTransform();

	if (isDebugCameraActive_ == true) { // デバッグカメラがアクティブなら
		// デバッグカメラの更新
		debugCamera_->Update();
		viewProjection_.matView_ = debugCamera_->GetViewProjection().matView_;
		viewProjection_.matProjection_ = debugCamera_->GetViewProjection().matProjection_;

		viewProjection_.TransferMatrix();

	}
	// アクティブでない
	else if (isDebugCameraActive_ == false) { // デバッグカメラがアクティブでない
		viewProjection_.matView_ = followCamera_->GetViewProjection().matView_;
		viewProjection_.matProjection_ = followCamera_->GetViewProjection().matProjection_;

		viewProjection_.TransferMatrix();
	}
}

void GameScene::Draw() {

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	skyDome_->Draw(viewProjection_);
	ground_->Draw(viewProjection_);
	for (std::unique_ptr<Enemy>& enemy : enemies_) {
		enemy->Draw(viewProjection_);
	}
	player_->Draw(viewProjection_);

	collisionManager_->Draw(viewProjection_);
}


void GameScene::AddEnemy() {
	std::vector<Model*> enemyModels = { modelEnemyBody_.get(), modelEnemyThurn_.get(), modelEnemyThurn_.get() };
	enemy_ = std::make_unique<Enemy>();
	enemy_->Init(enemyModels);
	enemies_.push_back(std::move(enemy_));
}

void GameScene::CheckAllCollisions() {
	// 衝突マネージャーのリセット
	collisionManager_->Reset();
	// コライダーをリストに登録
	collisionManager_->AddCollider(player_.get());
	for (const std::unique_ptr<Enemy>& enemy : enemies_) {
		collisionManager_->AddCollider(enemy.get());
	}
	collisionManager_->AddCollider(hummer_.get());
	// 衝突判定と応答
	collisionManager_->CheckAllCollisions();
}
