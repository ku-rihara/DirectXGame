#include "SceneManager.h"
#include"utility/ParticleEditor/ParticleManager.h"
#include"utility/ParameterEditor/GlobalParameter.h"
#include"input/Input.h"
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
    if (Input::GetInstance()->PushKey(DIK_E) && Input::GetInstance()->PushKey(DIK_ESCAPE)) {
        ChangeScene("EDITOR");
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

void SceneManager::SkyBoxDraw() {
    if (scene_) {
        scene_->SkyBoxDraw();
    }
 }
///==============================================
/// シーン切り替え
///==============================================
void SceneManager::ChangeScene(const std::string& scenemane) {
	assert(sceneFactory_);
	assert(!nextScene_);

	if (scene_) {
        // 現在のシーンを終了
        scene_.reset();
    }

	// 次のシーンを生成
	nextScene_ = std::unique_ptr<BaseScene>(sceneFactory_->CreateScene(scenemane));
	scene_ = std::move(nextScene_);
	scene_->Init();
  
	//パーティクルリセット
	ParticleManager::GetInstance()->ResetAllParticles();
	//// グローバル変数の読み込み
	GlobalParameter::GetInstance()->LoadFiles();
}
