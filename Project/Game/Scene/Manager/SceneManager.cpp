#include "SceneManager.h"
#include"utility/Particle/ParticleManager.h"
#include"utility/Editor/GlobalParameter.h"
#include <cassert>

// シングルトンインスタンスの取得
SceneManager* SceneManager::GetInstance() {
	static SceneManager instance; // 静的インスタンス
	return &instance;
}

SceneManager::~SceneManager() {
	// unique_ptrによる自動解放
}

///==============================================
/// 更新
///==============================================
void SceneManager::Update() {

	// 次のシーンが設定されている場合
	if (nextScene_) {
		scene_ = std::move(nextScene_);
		scene_->Init();
	}

	// 現在のシーンを更新
	if (scene_) {
		scene_->Update();
	}
}

///==============================================
/// モデル描画
///==============================================
void SceneManager::ModelDraw() {
	if (scene_) {
		scene_->ModelDraw();
	}
}

///==============================================
/// スプライト描画
///==============================================
void SceneManager::SpriteDraw() {
	if (scene_) {
		scene_->SpriteDraw();
	}
}

///==============================================
/// パーティクル描画
///==============================================
void SceneManager::ParticleDraw() {
	if (scene_) {
		scene_->ParticleDraw();
	}
}

///==============================================
/// シーン切り替え
///==============================================
void SceneManager::ChangeScene(const std::string& scenemane) {
	assert(sceneFactory_);
	assert(!nextScene_);

	// 次のシーンを生成
	nextScene_ = std::unique_ptr<BaseScene>(sceneFactory_->CreateScene(scenemane));
	nextScene_->Init();
	//パーティクルリセット
	ParticleManager::GetInstance()->ResetAllParticles();
	//// グローバル変数の読み込み
	GlobalParameter::GetInstance()->LoadFiles();
}
