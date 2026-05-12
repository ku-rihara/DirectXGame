#include "SceneManager.h"
using namespace KetaEngine;

// 3d
#include "3d/Object3D/Object3DRegistry.h"
#include "3D/AnimationObject3D/AnimationRegistry.h"
// base
#include "Base/Dx/DirectXCommon.h"
#include "Frame/Frame.h"
#include "Pipeline/PipelineManager.h"
// particle
#include "Particle/CPUParticle/ParticleManager.h"
#include "Particle/GPUParticle/GPUParticleManager.h"
// input
#include "input/Input.h"
// global parameter
#include "Editor/ParameterEditor/GlobalParameter.h"

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
    // シーン切り替えがあれば実行
    ApplyPendingSceneChange();

    // 次のシーンが設定されている場合
    if (Input::GetInstance()->PushKey(KeyboardKey::E) && Input::GetInstance()->PushKey(KeyboardKey::Escape)) {
        ChangeScene("EDITOR");
    }

    // 次のシーンが設定されている場合
    if (Input::GetInstance()->PushKey(KeyboardKey::R) && Input::GetInstance()->PushKey(KeyboardKey::T)) {
        ChangeScene("RESULT");
    }

    if (scene_) {
        scene_->Update();
    }

    // 登録されているオブジェクトを更新
    Object3DRegistry::GetInstance()->UpdateAll();
    AnimationRegistry::GetInstance()->UpdateAll(Frame::DeltaTimeRate());

    // パーティクル更新
    ParticleManager::GetInstance()->Update();
    GPUParticleManager::GetInstance()->Update();

    collisionManager_->Update();
}

void SceneManager::Debug() {
    if (scene_) {
        scene_->Debug();
    }
    if (collisionManager_) {
        collisionManager_->AdjustParam();
    }
}

void SceneManager::SkyBoxDraw() {
    if (scene_) {
        PipelineManager::GetInstance()->PreDraw(PipelineType::SkyBox, DirectXCommon::GetInstance()->GetCommandList());
        scene_->SkyBoxDraw();
        PipelineManager::GetInstance()->PreDraw(PipelineType::Object3D, DirectXCommon::GetInstance()->GetCommandList());
    }
}

///==============================================
/// シーン切り替え
///==============================================
void SceneManager::ChangeScene(const std::string& sceneName) {
    pendingSceneName_ = sceneName;
    if (!scene_) {
        ApplyPendingSceneChange();
    }
}

///==============================================
/// シーン切り替え実行
///==============================================
void SceneManager::ApplyPendingSceneChange() {
    if (pendingSceneName_.empty()) {
        return;
    }

    assert(sceneFactory_);

    if (scene_) {
        // 現在のシーンを終了
        scene_.reset();
    }

    if (collisionManager_) {
        collisionManager_.reset();
    }

    Object3DRegistry::GetInstance()->Clear();
    AnimationRegistry::GetInstance()->Clear();

    //// グローバル変数の読み込み
    GlobalParameter::GetInstance()->ResetAllRegister();
    GlobalParameter::GetInstance()->LoadFiles();

    // コリジョン
    collisionManager_ = std::make_unique<CollisionManager>();
    collisionManager_->Init();

    // 次のシーンを生成
    scene_ = std::unique_ptr<BaseScene>(sceneFactory_->CreateScene(pendingSceneName_));
    scene_->Init();

    // パーティクルリセット
    ParticleManager::GetInstance()->ResetAllParticles();

    // シーン名をクリア
    pendingSceneName_.clear();

    // シーン切り替え（ロード）にかかった時間を破棄するためタイマーをリセット
    Frame::Init();
}
