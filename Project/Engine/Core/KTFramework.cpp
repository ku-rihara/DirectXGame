#include "KTFramework.h"

using namespace KetaEngine;

// dx
#include "3d/ModelManager.h"
// particle
#include "Base/Descriptors/SrvManager.h"
#include "Base/Dx/DxRenderTarget.h"
#include "Particle/GPUParticle/GPUParticleManager.h"
#include "PostEffect/PostEffectRenderer.h"
#include "ShadowMap/ShadowMap.h"

// resource
#include "Core/ResourceLoader.h"
// Frame
#include "Frame/Frame.h"
// dx
#include "Base/Dx/DirectXCommon.h"

// utility
#include "../utility/Log/Log.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
// imGui
#include "2D/ImGuiManager.h"
#include <chrono>
#include <format>
#include <imgui.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

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

    // ロード完了後にタイマーをリセット
    Frame::Init();

    using Clock = std::chrono::steady_clock;
    auto ms = [](Clock::time_point a, Clock::time_point b) {
        return std::chrono::duration<float, std::milli>(b - a).count();
    };

    static int slowFrameCount = 0;

    // ウィンドウのxボタンが押されるまでループ
    while (engineCore_->ProcessMessage() == 0) {

        auto t0 = Clock::now();

        /// フレーム開始前処理: FixFPS + ResetDeltaTime
        Frame::Update();
        auto t1 = Clock::now();

        // BeginFrame: WaitForNextFrame(FLOW) + ImGui + Input
        engineCore_->BeginFrame();
        auto t2 = Clock::now();

        // 更新
        Update();
        ShadowMap::GetInstance()->UpdateLightMatrix();
        auto t3 = Clock::now();

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
        auto t4 = Clock::now();

        /// フレームの終了: ExecuteCommand → Present(1,VSync) → WaitForGPU
        engineCore_->EndFrame();
        auto t5 = Clock::now();

        // シーン遷移直後: GPUがアイドル後のP-state低下をP0に戻してから本番ループへ
        if (pSceneManager_->IsJustTransitioned()) {
            pSceneManager_->ClearTransitionFlag();
            RunGpuWarmup(8);
            DwmFlush(); // DWMのVSync境界に同期してPresent位相を正規化
            Frame::Init(); // ウォームアップ分の経過時間を除外
        }

        // 各フェーズのタイミングを記録
        frameTimings_.frameMs    = ms(t0, t5);
        frameTimings_.fixFpsMs   = ms(t0, t1);
        frameTimings_.beginMs    = ms(t1, t2);
        frameTimings_.updateMs   = ms(t2, t3);
        frameTimings_.drawMs     = ms(t3, t4);
        frameTimings_.endFrameMs = ms(t4, t5);
        // FixFPS除いた実作業時間
        frameTimings_.workMs     = ms(t1, t5);

        // 低速フレームをログに記録（診断用）
        if (frameTimings_.workMs > 16.0f) {
            ++slowFrameCount;
            if (slowFrameCount <= 120) {
                KetaEngine::Log::Warn(std::format(
                    "[SlowFrame #{}] work={:.2f}ms (fixFps={:.2f} begin={:.2f} update={:.2f} draw={:.2f} end={:.2f})",
                    slowFrameCount,
                    frameTimings_.workMs,
                    frameTimings_.fixFpsMs,
                    frameTimings_.beginMs,
                    frameTimings_.updateMs,
                    frameTimings_.drawMs,
                    frameTimings_.endFrameMs));
            }
        } else {
            slowFrameCount = 0;
        }
    }
    Finalize();
}

// ========================================================
// 更新処理
// ========================================================
void KTFramework::Update() {

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

    ImGui::Begin("FPS / Frame Timing");

    // FPS
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(100, 255, 100, 255));
    ImGui::Text("FPS: %.1f", io.Framerate);
    ImGui::PopStyleColor();

    ImGui::Separator();

    // 各フェーズ (ms)
    ImGui::Text("Frame total : %5.2f ms", frameTimings_.frameMs);
    ImGui::Text("  FixFPS    : %5.2f ms", frameTimings_.fixFpsMs);
    ImGui::Text("  BeginFrame: %5.2f ms  (WaitNextFrame+ImGui+Input)", frameTimings_.beginMs);
    ImGui::Text("  Update    : %5.2f ms", frameTimings_.updateMs);
    ImGui::Text("  Draw      : %5.2f ms", frameTimings_.drawMs);

    ImGui::Text("  EndFrame  : %5.2f ms  (Present(VSync)+GPU)", frameTimings_.endFrameMs);

    ImGui::Separator();

    // 実作業時間 
    constexpr float kBudget = 16.666f;
    bool over = frameTimings_.workMs > kBudget;
    ImGui::PushStyleColor(ImGuiCol_Text, over ? IM_COL32(255, 60, 60, 255) : IM_COL32(60, 255, 60, 255));
    ImGui::Text("Work (excl.FixFPS): %5.2f ms  %s",
        frameTimings_.workMs,
        over ? "<<< OVER BUDGET (30fps)" : "OK (60fps)");
    ImGui::PopStyleColor();

    ImGui::Separator();
    ImGui::TextDisabled("EndFrame = ExecuteCommand + Present + WaitForGPU");
    ImGui::TextDisabled("WaitForGPU: Debug layer ON -> ~30ms, OFF -> ~3ms");

    ImGui::End();
#endif
}

// ========================================================
// GPUウォームアップ
// ========================================================
void KTFramework::RunGpuWarmup(int numFrames) {
    auto* dxCommon = DirectXCommon::GetInstance();
    auto* srvMgr   = SrvManager::GetInstance();

    auto t0 = std::chrono::steady_clock::now();
    for (int i = 0; i < numFrames; ++i) {
        srvMgr->PreDraw();            // デスクリプタヒープをコマンドリストに設定
        dxCommon->PreRenderTexture(); // オフスクリーンRT: RENDER_TARGET + クリア
        dxCommon->PreDraw();          // RT→SRV、バックバッファ→RENDER_TARGET
        dxCommon->PostDraw();         // RENDER_TARGET→PRESENT + Execute + Present + Wait + Reset
    }

    float elapsed = std::chrono::duration<float, std::milli>(
        std::chrono::steady_clock::now() - t0).count();
    KetaEngine::Log::Info(std::format("[GpuWarmup] {} frames in {:.1f}ms", numFrames, elapsed));
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
