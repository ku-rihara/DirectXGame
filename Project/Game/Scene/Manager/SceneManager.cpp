#include "SceneManager.h"
#include"utility/ParticleEditor/ParticleManager.h"
#include"utility/ParameterEditor/GlobalParameter.h"
#include"3d/Object3DRegistry.h"
#include"Animation/AnimationRegistry.h"
#include"input/Input.h"
#include <cassert>

// シングルトンインスタンスの取得
SceneManager* SceneManager::GetInstance() {
	static SceneManager instance; 
	return &instance;
}

SceneManager::~SceneManager() {
	
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

void SceneManager::DrawShadow() {
     if (scene_) {
         scene_->DrawShadow();
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
    
	Object3DRegistry::GetInstance()->Clear();
    AnimationRegistry::GetInstance()->Clear();

    //// グローバル変数の読み込み
    GlobalParameter::GetInstance()->BindResetAll();
    GlobalParameter::GetInstance()->LoadFiles();

	// 次のシーンを生成
	nextScene_ = std::unique_ptr<BaseScene>(sceneFactory_->CreateScene(scenemane));
	scene_ = std::move(nextScene_);
	scene_->Init();
  
	//パーティクルリセット
	ParticleManager::GetInstance()->ResetAllParticles();
}
