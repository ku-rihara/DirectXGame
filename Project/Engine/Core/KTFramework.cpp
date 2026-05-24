#include "KTFramework.h"

using namespace KetaEngine;

// dx
#include "3d/ModelManager.h"
#include "Base/Dx/DxRenderTarget.h"
#include "PostEffect/PostEffectRenderer.h"
#include "ShadowMap/ShadowMap.h"

// resource
#include "Core/ResourceLoader.h"
// Frame
#include "Frame/Frame.h"
// particle
#include "Particle/GPUParticle/GPUParticleManager.h"
// dx
#include "Base/Dx/DirectXCommon.h"

// utility
#include "../utility/Log/Log.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
// imGui
#include "2D/ImGuiManager.h"
#include <imgui.h>
#include <chrono>
#include <format>

const char kWindowTitle[] = "LE4A_13_クリハラ_ケイタ";

// ========================================================
// 初期化
// ========================================================
void KTFramework::Init() {

    engineCore_ = std::make_unique<EngineCore>();

    // グローバル変数の読み込み
    GlobalParameter::GetInstance()->LoadFiles();

#if defined(_DEBUG) || defined(DEVELOPMENT)
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

    /// 全リソース一括ロード
    ResourceLoader::LoadAllResources();
}

// ========================================================
// 実行
// ========================================================
void KTFramework::Run() {
    Init(); /// 初期化

    // 低FPS検出用
    static constexpr float kSlowFrameThresholdMs = 30.0f;
    static constexpr int   kSlowFrameLogInterval = 60; // 60フレームに1回まとめてログ
    int slowFrameCount = 0;
    float slowFrameWorstMs = 0.0f;

    // ウィンドウのxボタンが押されるまでループ
    while (engineCore_->ProcessMessage() == 0) {
        auto frameStart = std::chrono::steady_clock::now();

        // フレームの開始
        Frame::Update();
        engineCore_->BeginFrame();

        auto workStart = std::chrono::steady_clock::now();

        // 更新
        Update();
        ShadowMap::GetInstance()->UpdateLightMatrix();

        auto* dx = DirectXCommon::GetInstance();

        // 影描画
        dx->BeginTimestamp(0);
        DrawShadow();
        dx->EndTimestamp(0);

        /// 描画前処理
        engineCore_->PreRenderTexture();

        // オフジェクト、スプライト描画
        dx->BeginTimestamp(1);
        Draw();
        dx->EndTimestamp(1);

        /// 描画前処理
        engineCore_->PreDraw();

        // ポストエフェクト描画
        dx->BeginTimestamp(2);
        DrawPostEffect();
        dx->EndTimestamp(2);

        dx->ResolveTimestamps();

        float workMs = std::chrono::duration<float, std::milli>(
            std::chrono::steady_clock::now() - workStart).count();

        /// フレームの終了
        engineCore_->EndFrame();

        // シーン遷移直後：GPUクロックを安定させるためウォームアップフレームを実行
     
        if (pSceneManager_->IsJustTransitioned()) {
            pSceneManager_->ClearTransitionFlag();

            static constexpr int   kGpuWarmupFrames = 10;
            static constexpr float kGpuWarmupBudgetMs = 500.0f;
            auto warmupStart = std::chrono::steady_clock::now();
            auto* dxWarm = DirectXCommon::GetInstance();

            for (int w = 0; w < kGpuWarmupFrames; w++) {
                float elapsed = std::chrono::duration<float, std::milli>(
                    std::chrono::steady_clock::now() - warmupStart).count();
                if (elapsed >= kGpuWarmupBudgetMs) break;

                Frame::Update();
                engineCore_->BeginFrame();
                ShadowMap::GetInstance()->UpdateLightMatrix();
                dxWarm->BeginTimestamp(0); DrawShadow();      dxWarm->EndTimestamp(0);
                engineCore_->PreRenderTexture();
                dxWarm->BeginTimestamp(1); Draw();             dxWarm->EndTimestamp(1);
                engineCore_->PreDraw();
                dxWarm->BeginTimestamp(2); DrawPostEffect();   dxWarm->EndTimestamp(2);
                dxWarm->ResolveTimestamps();
                engineCore_->EndFrame();
            }

            Frame::Init(); // ウォームアップ中の時間でdeltaTimeが跳ねないようリセット
            Log::Info(std::format(
                "[GPU Warmup] Done | shadow={:.1f}ms main={:.1f}ms total={:.0f}ms",
                dxWarm->GetTimestampMs(0),
                dxWarm->GetTimestampMs(1),
                std::chrono::duration<float, std::milli>(
                    std::chrono::steady_clock::now() - warmupStart).count()));
        }

        float totalMs = std::chrono::duration<float, std::milli>(
            std::chrono::steady_clock::now() - frameStart).count();

        // 低FPS検出ログ
        if (totalMs > kSlowFrameThresholdMs) {
            slowFrameCount++;
            if (totalMs > slowFrameWorstMs) slowFrameWorstMs = totalMs;

            if (slowFrameCount % kSlowFrameLogInterval == 1) {
                auto* pm = GPUParticleManager::GetInstance();
                auto* dxLog = DirectXCommon::GetInstance();
                auto* sm = pSceneManager_;
                Log::Warn(std::format(
                    "[SLOW FRAME] total={:.1f}ms | work={:.1f}ms | fps~={:.0f} | particles={}/{} | shadow={:.1f}ms main={:.1f}ms postfx={:.1f}ms | cpu: scene={:.1f}ms reg={:.1f}ms ptcl={:.1f}ms col={:.1f}ms",
                    totalMs,
                    workMs,
                    totalMs > 0.0f ? 1000.0f / totalMs : 0.0f,
                    pm->GetActiveGroupCount(),
                    pm->GetTotalGroupCount(),
                    dxLog->GetTimestampMs(0),
                    dxLog->GetTimestampMs(1),
                    dxLog->GetTimestampMs(2),
                    sm->GetLastSceneUpdateMs(),
                    sm->GetLastRegistryMs(),
                    sm->GetLastParticleMs(),
                    sm->GetLastCollisionMs()));
            }
        } else {
            // 正常に回復したらカウントリセット
            if (slowFrameCount > 0) {
                auto* pm = GPUParticleManager::GetInstance();
                auto* dxLog = DirectXCommon::GetInstance();
                Log::Info(std::format(
                    "[FRAME RECOVERY] {} slow frames (worst={:.1f}ms), now normal ({:.1f}ms) | particles={}/{} | shadow={:.1f}ms main={:.1f}ms postfx={:.1f}ms",
                    slowFrameCount, slowFrameWorstMs, totalMs,
                    pm->GetActiveGroupCount(),
                    pm->GetTotalGroupCount(),
                    dxLog->GetTimestampMs(0),
                    dxLog->GetTimestampMs(1),
                    dxLog->GetTimestampMs(2)));
                slowFrameCount = 0;
                slowFrameWorstMs = 0.0f;
            }
        }
    }
    Finalize();
}

// ========================================================
// 更新処理
// ========================================================
void KTFramework::Update() {

    /// グローバル変数の更新
#if defined(_DEBUG) || defined(DEVELOPMENT)
    GlobalParameter::GetInstance()->SyncAll();
#endif
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
    KetaEngine::Log::Finalize();
    if (pSceneManager_) {
        pSceneManager_->Finalize();
    }
    // ライブラリの終了
    engineCore_->Finalize();
}

// ========================================================
// FPS表示
// ========================================================
void KTFramework::DisplayFPS() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    ImGuiIO& io = ImGui::GetIO();

    // FPSウィンドウ
    ImGui::Begin("FPS");
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(100, 255, 100, 255));
    ImGui::Text("FPS: %.1f", io.Framerate);
    ImGui::PopStyleColor();
    ImGui::Separator();
    ImGui::Separator();
    auto* pm = GPUParticleManager::GetInstance();
    ImGui::Text("Particles: %d/%d groups", pm->GetActiveGroupCount(), pm->GetTotalGroupCount());
    ImGui::Separator();
    auto* dxImGui = DirectXCommon::GetInstance();
    ImGui::Text("Shadow:  %.2fms", dxImGui->GetTimestampMs(0));
    ImGui::Text("Main3D:  %.2fms", dxImGui->GetTimestampMs(1));
    ImGui::Text("PostFx:  %.2fms", dxImGui->GetTimestampMs(2));
    ImGui::End();
#endif
}

// ========================================================
// ゲームビュー表示
// ========================================================
void KTFramework::DisplayGameView() {
#if defined(_DEBUG) || defined(DEVELOPMENT)

    // ポストエフェクト適用済みならピンポン、未適用ならシーンRTのSRVを取得
    D3D12_GPU_DESCRIPTOR_HANDLE srvHandle =
        PostEffectRenderer::GetInstance()->GetPostProcessedSRVHandle();

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
