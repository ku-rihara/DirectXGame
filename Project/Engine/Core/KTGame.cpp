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
#include "3D/RibbonTrail/RibbonTrailManager.h"
#include "Particle/GPUParticle/GPUParticleManager.h"
#include "Base/Dx/DirectXCommon.h"
#include "Base/WinApp.h"

// utility
#include "Collider/CollisionManager.h"

// =============================================================
// 初期化処理
// =============================================================
void KTGame::Init() {
    KTFramework::Init();

    RibbonTrailManager::GetInstance()->InitDistortion(
        DirectXCommon::GetInstance(),
        static_cast<uint32_t>(WinApp::kWindowWidth),
        static_cast<uint32_t>(WinApp::kWindowHeight));

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
    // リボントレイル描画
    RibbonTrailManager::GetInstance()->DrawAll(viewProjection);
    // 時空歪みパス（リボン + パーティクルを同一RTで描画）
    bool hasParticleDistortion = ParticleManager::GetInstance()->HasDistortionParticles();
    RibbonTrailManager::GetInstance()->DrawDistortionPass(viewProjection, hasParticleDistortion);
    ParticleManager::GetInstance()->DrawDistortion(viewProjection);
    RibbonTrailManager::GetInstance()->CloseDistortionPass();

    // --------------------------------------------------------------------------
    /// スプライト描画
    // --------------------------------------------------------------------------
    SpriteRegistry::GetInstance()->DrawAll();

    // --------------------------------------------------------------------------
    // スクリーン座標系パーティクル描画
    // --------------------------------------------------------------------------
    ParticleManager::GetInstance()->DrawScreenPos();
}

void KTGame::DrawShadow() {
    const ViewProjection& viewProjection = pSceneManager_->GetScene()->GetViewProjection();

    // 影描画前処理
    ShadowMap::GetInstance()->PreDraw();
    // 影描画
    Object3DRegistry::GetInstance()->DrawAllShadow(viewProjection);
    AnimationRegistry::GetInstance()->DrawAllShadow(viewProjection);
    // 影描画後処理
    ShadowMap::GetInstance()->PostDraw();
}

void KTGame::DrawPostEffect() {

    /// commandList取得
    ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();
    PostEffectRenderer::GetInstance()->Draw(commandList);
    RibbonTrailManager::GetInstance()->ApplyDistortionEffect(commandList);
}

// =============================================================
// 終了処理
// =============================================================
void KTGame::Finalize() {
    KTFramework::Finalize();
}
