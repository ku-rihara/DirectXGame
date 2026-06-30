#include <format>
#include "SceneManager.h"
using namespace KetaEngine;

// 3d
#include "3D/AnimationObject3D/AnimationRegistry.h"
#include "3D/Object3D/Object3DRegistry.h"
// base
#include "Base/Dx/DirectXCommon.h"
#include "Base/Dx/DxCommand.h"
#include "Frame/Frame.h"
#include "Pipeline/PipelineManager.h"
// particle
#include "2D/SpriteRegistry.h"
#include "3D/RibbonTrail/RibbonTrailManager.h"
#include "Particle/CPUParticle/ParticleManager.h"
#include "Particle/GPUParticle/GPUParticleManager.h"
// input
#include "Input/Input.h"
// global parameter
#include "Editor/ParameterEditor/GlobalParameter.h"
// log
#include "Utility/Log/Log.h"

#include <cassert>
#include <chrono>

// シングルトンインスタンスの取得
SceneManager* SceneManager::GetInstance() {
    static SceneManager instance;
    return &instance;
}

void SceneManager::Finalize() {

    // シーンの破棄
    if (scene_) {
        scene_.reset();
    }

    // コリジョンマネージャの破棄
    if (collisionManager_) {
        collisionManager_.reset();
    }

    // 登録されているオブジェクトの破棄
    ClearAllRegistries();
}

///==============================================
/// 更新
///==============================================
void SceneManager::Update() {
    // シーン切り替えがあれば実行
    ApplyPendingSceneChange();

    // 次のシーンが設定されている場合
    if (Input::GetInstance()->PushKey(KeyboardKey::E) && Input::GetInstance()->PushKey(KeyboardKey::Escape)) {
        ChangeScene("BATTLE_TEST");
    }

    // 次のシーンが設定されている場合
    if (Input::GetInstance()->PushKey(KeyboardKey::R) && Input::GetInstance()->PushKey(KeyboardKey::T)) {
        ChangeScene("RESULT");
    }

    if (scene_) { scene_->Update(); }

    Object3DRegistry::GetInstance()->UpdateAll();
    AnimationRegistry::GetInstance()->UpdateAll(Frame::DeltaTimeRate());

    ParticleManager::GetInstance()->Update();
    GPUParticleManager::GetInstance()->Update();

    collisionManager_->Update();
}

void SceneManager::Debug() {

    // シーンごとのデバッグ表示
    if (scene_) {
        scene_->Debug();
    }

    // 登録されているオブジェクトのデバッグ表示
    if (collisionManager_) {
        collisionManager_->AdjustParam();
    }
}

void SceneManager::SkyBoxDraw() {

    // シーンのスカイボックス描画
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
    // シーン切り替えを予約
    pendingSceneName_ = sceneName;
}

///==============================================
/// シーン切り替え実行
///==============================================
void SceneManager::ApplyPendingSceneChange() {

    // シーン切り替えが予約されていない場合は何もしない
    if (pendingSceneName_.empty()) {
        return;
    }

    // シーン工場が設定されていない場合はエラー
    assert(sceneFactory_);

    auto transitionStart = std::chrono::steady_clock::now();
    KetaEngine::Log::Info(std::format("[SceneTransition] START: {} -> {}",
        scene_ ? "prev" : "none", pendingSceneName_));

    // 現在のシーンを終了
    if (scene_) {
        scene_.reset();
    }

    // コリジョンマネージャのリセット
    if (collisionManager_) {
        collisionManager_.reset();
    }

    // GPUが現在実行中のコマンドを完了するのを待つ
    auto t0 = std::chrono::steady_clock::now();
    DirectXCommon::GetInstance()->GetDxCommand()->WaitForGPU();
    float waitGPUMs = std::chrono::duration<float, std::milli>(std::chrono::steady_clock::now() - t0).count();

    // 登録されているオブジェクトの破棄
    ClearAllRegistries();

    // グローバル変数の登録全解除
    GlobalParameter::GetInstance()->ResetAllRegister();
    t0 = std::chrono::steady_clock::now();
    GlobalParameter::GetInstance()->LoadFiles();
    float loadFilesMs = std::chrono::duration<float, std::milli>(std::chrono::steady_clock::now() - t0).count();

    // パーティクルリセット
    ParticleManager::GetInstance()->ResetAllParticles();
    GPUParticleManager::GetInstance()->ResetAllParticles();

    // コリジョン生成
    collisionManager_ = std::make_unique<CollisionManager>();
    collisionManager_->Init();

    // 次のシーンを生成
    auto sceneInitStart = std::chrono::steady_clock::now();
    scene_ = sceneFactory_->CreateScene(pendingSceneName_);
    scene_->Init();
    float sceneInitMs = std::chrono::duration<float, std::milli>(
        std::chrono::steady_clock::now() - sceneInitStart).count();

    int32_t particleGroups = GPUParticleManager::GetInstance()->GetTotalGroupCount();

    // シーン遷移後にタイマーをリセット。遷移中の長時間ブロックにより
    Frame::Init();

    float totalMs = std::chrono::duration<float, std::milli>(
        std::chrono::steady_clock::now() - transitionStart).count();
    KetaEngine::Log::Info(std::format(
        "[SceneTransition] END: {} | waitGPU={:.1f}ms loadFiles={:.1f}ms sceneInit={:.1f}ms total={:.1f}ms particles={}groups",
        pendingSceneName_, waitGPUMs, loadFilesMs, sceneInitMs, totalMs, particleGroups));

    // シーン名をクリア
    pendingSceneName_.clear();
    justTransitioned_ = true;
}

void SceneManager::ClearAllRegistries() {
    Object3DRegistry::GetInstance()->Clear();
    AnimationRegistry::GetInstance()->Clear();
    RibbonTrailManager::GetInstance()->Clear();
    SpriteRegistry::GetInstance()->Clear();
}
