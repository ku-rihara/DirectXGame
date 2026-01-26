#include "KTGame.h"

using namespace KetaEngine;
/// Scene
#include "2D/SpriteRegistry.h"
#include "3D/Object3D/Object3DRegistry.h"
#include "3D/AnimationObject3D/AnimationRegistry.h"
#include "PostEffect/PostEffectRenderer.h"
#include "Scene/Factory/SceneFactory.h"
#include "Particle/CPUParticle/ParticleManager.h"
#include "3D/Line3D/Line3DManager.h"
#include "Particle/GPUParticle/GPUParticleManager.h"

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

 
    const ViewProjection& viewProjection   = pSceneManager_->GetScene()->GetViewProjection();
    // --------------------------------------------------------------------------
    /// SkyBox描画
    // --------------------------------------------------------------------------
    /// ゲームシーン描画
    pSceneManager_->SkyBoxDraw();

    // --------------------------------------------------------------------------
    // モデル描画
    // --------------------------------------------------------------------------
   
    // オブジェクト描画
    Object3DRegistry::GetInstance()->DrawAll(viewProjection);
    // アニメーション描画
    AnimationRegistry::GetInstance()->DrawAll(viewProjection);
    // パーティクル描画
    ParticleManager::GetInstance()->Draw(viewProjection);
    // GPUパーティクル描画
    GPUParticleManager::GetInstance()->Draw(viewProjection);
    // ライン描画
    Line3DManager::GetInstance()->DrawAll(viewProjection);
  
    // --------------------------------------------------------------------------
    /// スプライト描画
    // --------------------------------------------------------------------------
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
    PostEffectRenderer::GetInstance()->Draw(commandList);
}

// =============================================================
// 終了処理
// =============================================================
void KTGame::Finalize() {
    KTFramework::Finalize();
}
