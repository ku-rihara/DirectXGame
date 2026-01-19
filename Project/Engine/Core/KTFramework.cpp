#include "KTFramework.h"

using namespace KetaEngine;

// dx
#include "Dx/DxReleaseChecker.h"
// dx
#include "Dx/DxReleaseChecker.h"
#include "ShadowMap/ShadowMap.h"

/// utility
#include "Editor/ParameterEditor/GlobalParameter.h"

#include "Frame/Frame.h"
/// imgui
#include "base/TextureManager.h"
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
    /// FPS表示
    DisplayFPS();
    /// グローバル変数の更新
    GlobalParameter::GetInstance()->SyncAll();
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

    // 位置
    ImGui::SetNextWindowPos(ImVec2(1230, 0), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.0f); // 背景を完全透明に設定

    // ウィンドウフラグを設定
    ImGui::Begin("FPS Overlay", nullptr,
        ImGuiWindowFlags_NoTitleBar |           // タイトルバーを非表示
            ImGuiWindowFlags_NoResize |         // リサイズを禁止
            ImGuiWindowFlags_NoMove |           // ウィンドウの移動を禁止
            ImGuiWindowFlags_NoScrollbar |      // スクロールバーを非表示
            ImGuiWindowFlags_NoCollapse |       // 折りたたみボタンを非表示
            ImGuiWindowFlags_AlwaysAutoResize | // 必要なサイズに自動調整
            ImGuiWindowFlags_NoBackground       // 背景を非表示
    );

    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(100, 255, 100, 255)); /// 色設定（緑）
    ImGui::Text("%.1f", io.Framerate);
    ImGui::PopStyleColor();

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
}
