#include "KTGame.h"
/// Scene
#include "2d/SpriteRegistry.h"
#include "3d/Object3DRegistry.h"
#include "Animation/AnimationRegistry.h"
#include "Pipeline/PipelineManager.h"
#include "PostEffect/PostEffectRenderer.h"
#include "Scene/Factory/SceneFactory.h"
#include "utility/ParticleEditor/ParticleManager.h"
// renderer
#include "base/SkyBoxRenderer.h"
// utility
#include "Collider/CollisionManager.h"

// =============================================================
// 初期化処理
// =============================================================
void KTGame::Init() {
    KTFramework::Init();

    // シーン生成
    sceneFactory_ = std::make_unique<SceneFactory>();

    // タイトルから始める
    pSceneManager_ = SceneManager::GetInstance();
    pSceneManager_->SetSceneFactory(sceneFactory_.get());
    pSceneManager_->ChangeScene("TITLE");
    /*pSceneManager_->ChangeScene("GAMEPLAY");*/
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
    const ViewProjection& viewProjection   = pSceneManager_->GetScene()->GetViewProjection();
    // --------------------------------------------------------------------------
    /// SkyBox描画
    // --------------------------------------------------------------------------
    SkyBoxRenderer::GetInstance()->PreDraw(commandList);
    /// ゲームシーン描画
    pSceneManager_->SkyBoxDraw();

    // --------------------------------------------------------------------------
    // モデル描画
    // --------------------------------------------------------------------------
    PipelineManager::GetInstance()->PreDraw(PipelineType::Object3D, commandList);

    // オブジェクト、アニメーション、パーティクル描画
    Object3DRegistry::GetInstance()->DrawAll(viewProjection);
    AnimationRegistry::GetInstance()->DrawAll(viewProjection);
    ParticleManager::GetInstance()->Draw(viewProjection);

    /// コリジョン描画
    collisionManager_->Draw(viewProjection);

    // --------------------------------------------------------------------------
    /// スプライト描画
    // --------------------------------------------------------------------------
    PipelineManager::GetInstance()->PreDraw(PipelineType::Sprite, commandList);
    SpriteRegistry::GetInstance()->DrawAll();
}

void KTGame::DrawShadow() {
    const ViewProjection& viewProjection = pSceneManager_->GetScene()->GetViewProjection();

    // 影描画前処理
    ShadowMap::GetInstance()->PreDraw();
    // 影描画
    Object3DRegistry::GetInstance()->DrawAllShadow(viewProjection);
    // 影描画後処理
    ShadowMap::GetInstance()->PostDraw();
}

void KTGame::DrawPostEffect() {

    /// commandList取得
    ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();

    PostEffectRenderer::GetInstance()->DrawImGui();
    PostEffectRenderer::GetInstance()->Draw(commandList);
}

// =============================================================
// 終了処理
// =============================================================
void KTGame::Finalize() {
    KTFramework::Finalize();
}
