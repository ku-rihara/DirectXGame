#include "SceneManager.h"
#include "3d/Object3DRegistry.h"
#include "Animation/AnimationRegistry.h"
#include "Dx/DirectXCommon.h"
#include "Frame/Frame.h"
#include "GPUParticle/GPUParticleManager.h"
#include "input/Input.h"
#include "Pipeline/PipelineManager.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Editor/ParticleEditor/ParticleManager.h"

#include "3d/Object3DRegistry.h"
#include "Animation/AnimationRegistry.h"
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
    if (Input::GetInstance()->PushKey(KeyboardKey::E) && Input::GetInstance()->PushKey(KeyboardKey::Escape)) {
        ChangeScene("EDITOR");
    }

    // 現在のシーンを更新
    if (scene_) {
        Debug();
        scene_->Update();
    }

    // 登録されているオブジェクトを更新
    Object3DRegistry::GetInstance()->UpdateAll(Frame::DeltaTimeRate());
    AnimationRegistry::GetInstance()->UpdateAll(Frame::DeltaTimeRate());

    // パーティクル更新
    ParticleManager::GetInstance()->Update();
    GPUParticleManager::GetInstance()->Update();
}

void SceneManager::Debug() {
    scene_->Debug();

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
    assert(sceneFactory_);
    assert(!nextScene_);

    if (scene_) {
        // 現在のシーンを終了
        scene_.reset();
    }

    Object3DRegistry::GetInstance()->Clear();
    AnimationRegistry::GetInstance()->Clear();

    //// グローバル変数の読み込み
    GlobalParameter::GetInstance()->ResetAllRegister();
    GlobalParameter::GetInstance()->LoadFiles();

    // 次のシーンを生成
    nextScene_ = std::unique_ptr<BaseScene>(sceneFactory_->CreateScene(sceneName));
    scene_     = std::move(nextScene_);
    scene_->Init();

    // パーティクルリセット
    ParticleManager::GetInstance()->ResetAllParticles();
}
