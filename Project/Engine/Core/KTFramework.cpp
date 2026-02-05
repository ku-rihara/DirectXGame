#include "KTFramework.h"

using namespace KetaEngine;

// dx
#include "Base/Dx/DxRenderTarget.h"
#include "PostEffect/PostEffectRenderer.h"
#include"3d/ModelManager.h"
#include "ShadowMap/ShadowMap.h"

// utility
#include "Editor/ParameterEditor/GlobalParameter.h"
// imGui
#include "2D/ImGuiManager.h"
#include <imgui.h>

const char kWindowTitle[] = "LE3A_11_クリハラ_ケイタ";

// ========================================================
// 初期化
// ========================================================
void KTFramework::Init() {

    engineCore_ = std::make_unique<EngineCore>();

    // グローバル変数の読み込み
    GlobalParameter::GetInstance()->LoadFiles();

#ifdef _DEBUG
    // Debug時：ウィンドウは大きく、ゲーム描画は1280x720
    engineCore_->Initialize(
        kWindowTitle,
        KetaEngine::WinApp::kDebugWindowWidth,
        KetaEngine::WinApp::kDebugWindowHeight,
        KetaEngine::WinApp::kWindowWidth,
        KetaEngine::WinApp::kWindowHeight);
#else
    // Release時：ウィンドウもゲーム描画も1280x720
    engineCore_->Initialize(kWindowTitle, KetaEngine::WinApp::kWindowWidth, KetaEngine::WinApp::kWindowHeight);
#endif

    LoadAllTexture();
}

// ========================================================
// 実行
// ========================================================
void KTFramework::Run() {
    Init(); /// 初期化

    // ウィンドウのxボタンが押されるまでループ
    while (engineCore_->ProcessMessage() == 0) {
        // フレームの開始
        engineCore_->BeginFrame();

        // 更新
        Update();
        ShadowMap::GetInstance()->UpdateLightMatrix();

        // 影描画
        DrawShadow();

        /// 描画前処理
        engineCore_->PreRenderTexture();

        // オフジェクト、スプライト描画
        Draw();

        /// 描画前処理
        engineCore_->PreDraw();

        // ポストエフェクト描画
        DrawPostEffect();

        /// フレームの終了
        engineCore_->EndFrame();
    }
    Finalize();
}

// ========================================================
// 更新処理
// ========================================================
void KTFramework::Update() {

    /// グローバル変数の更新
    GlobalParameter::GetInstance()->SyncAll();
    // デバッグ処理
    Debug();
    /// ゲームシーンの毎フレーム処理
    pSceneManager_->Update();
}

void KTFramework::Debug() {
    ImGuiManager* imGuiManager = ImGuiManager::GetInstance();

    // フルスクリーンモードの場合
    if (imGuiManager->IsFullScreenMode()) {
        return;
    }

    /// FPS表示
    DisplayFPS();
    /// ゲームビューウィンドウを表示
    DisplayGameView();
    pSceneManager_->Debug();
}

// ========================================================
// 解放
// ========================================================
void KTFramework::Finalize() {
    // ライブラリの終了
    engineCore_->Finalize();
}

// ========================================================
// FPS表示
// ========================================================
void KTFramework::DisplayFPS() {
#ifdef _DEBUG
    ImGuiIO& io = ImGui::GetIO();

    // FPSウィンドウ
    ImGui::Begin("FPS");
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(100, 255, 100, 255));
    ImGui::Text("FPS: %.1f", io.Framerate);
    ImGui::PopStyleColor();
    ImGui::End();
#endif
}

// ========================================================
// ゲームビュー表示
// ========================================================
void KTFramework::DisplayGameView() {
#ifdef _DEBUG
   
    // レンダーテクスチャのSRVを取得
    D3D12_GPU_DESCRIPTOR_HANDLE srvHandle =
        DirectXCommon::GetInstance()->GetDxRenderTarget()->GetRenderTextureSRVHandle();

    // 通常モード
    ImGui::Begin("Game View");

    // ウィンドウのコンテンツ領域サイズを取得
    ImVec2 windowSize = ImGui::GetContentRegionAvail();

    // アスペクト比を維持してテクスチャを表示
    float aspectRatio = WinApp::aspectRatio; 
    ImVec2 imageSize  = windowSize;

    if (windowSize.x / windowSize.y > aspectRatio) {
        imageSize.x = windowSize.y * aspectRatio;
    } else {
        imageSize.y = windowSize.x / aspectRatio;
    }

    // 中央寄せ
    ImVec2 cursorPos = ImGui::GetCursorPos();
    cursorPos.x += (windowSize.x - imageSize.x) * 0.5f;
    cursorPos.y += (windowSize.y - imageSize.y) * 0.5f;
    ImGui::SetCursorPos(cursorPos);

    // ゲーム画面を表示
    ImGui::Image((ImTextureID)srvHandle.ptr, imageSize);

    ImGui::End();
#endif
}


void KTFramework::LoadAllTexture() {
    TextureManager::GetInstance()->LoadTexture("Resources/Texture/uvChecker.png");
    TextureManager::GetInstance()->LoadTexture("Resources/Texture/smoke.png");
    TextureManager::GetInstance()->LoadTexture("Resources/Texture/circle.png");
    TextureManager::GetInstance()->LoadTexture("Resources/Texture/star.png");
    TextureManager::GetInstance()->LoadTexture("Resources/Texture/HitEffectCenter.png");
    TextureManager::GetInstance()->LoadTexture("Resources/Texture/hitEffect.png");
    TextureManager::GetInstance()->LoadTexture("Resources/Texture/HowToOperate.png");
    TextureManager::GetInstance()->LoadTexture("Resources/Model/AudienceSeats/audienceSeats.png");
    TextureManager::GetInstance()->LoadTexture("Resources/Model/FieldSide/SideFiend.png");

    KetaEngine::ModelManager::GetInstance()->LoadModel("NormalEnemyWaiting.gltf");
    KetaEngine::ModelManager::GetInstance()->LoadModel("NormalEnemySpawn.gltf");
    KetaEngine::ModelManager::GetInstance()->LoadModel("NormalEnemyDiscovery.gltf");
    KetaEngine::ModelManager::GetInstance()->LoadModel("NormalEnemyRun.gltf");
    KetaEngine::ModelManager::GetInstance()->LoadModel("NormalEnemyAttack.gltf");

}
