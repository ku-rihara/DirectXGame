#include "KTFramework.h"

using namespace KetaEngine;

// dx
#include "PostEffect/PostEffectRenderer.h"
#include"Base/Dx/DxRenderTarget.h"
#include "ShadowMap/ShadowMap.h"

// utility
#include "Editor/ParameterEditor/GlobalParameter.h"
// imGui
#include <imgui.h>

const char kWindowTitle[] = "LE3A_11_クリハラ_ケイタ";

// ========================================================
// 初期化
// ========================================================
void KTFramework::Init() {

    engineCore_ = std::make_unique<EngineCore>();

    // グローバル変数の読み込み
    GlobalParameter::GetInstance()->LoadFiles();

    /// エンジン初期化
    engineCore_->Initialize(kWindowTitle, KetaEngine::WinApp::kWindowWidth, KetaEngine::WinApp::kWindowHeight);
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
    /// FPS表示
    DisplayFPS();
    /// グローバル変数の更新
    GlobalParameter::GetInstance()->SyncAll();

    /// ゲームビューウィンドウを表示
    DisplayGameView();

    // デバッグ処理
    Debug();
    /// ゲームシーンの毎フレーム処理
    pSceneManager_->Update();
}

void KTFramework::Debug() {
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
    ImGui::Text("Frame Time: %.3f ms", 1000.0f / io.Framerate);
    ImGui::PopStyleColor();
    ImGui::End();
#endif
}

// ========================================================
// ゲームビュー表示
// ========================================================
void KTFramework::DisplayGameView() {
#ifdef _DEBUG
    ImGui::Begin("Game View");

    // レンダーテクスチャのSRVを取得
    D3D12_GPU_DESCRIPTOR_HANDLE srvHandle =
        DirectXCommon::GetInstance()->GetDxRenderTarget()->GetRenderTextureSRVHandle();

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

