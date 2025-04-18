#include "KTFramework.h"
/// base
#include "base/Keta.h"
/// imgui
#include <imgui.h>
#include "utility/ParameterEditor/GlobalParameter.h"

#include "Frame/Frame.h"

const char kWindowTitle[] = "LE2A_11_クリハラ_ケイタ";

// ========================================================
// 実行
// ========================================================
void KTFramework::Run() {
    Init(); /// 初期化

    // ウィンドウのxボタンが押されるまでループ
    while (Keta::ProcessMessage() == 0) {
        Keta::BeginFrame(); /// フレームの開始

        Update(); /// 更新

		Keta::PreRenderTexture(); /// 描画前処理

        Draw(); /// 描画

		Keta::PreDraw(); /// 描画前処理

		DrawImGuietc(); /// 描画

        Keta::EndFrame(); /// フレームの終了
    }
    Finalize();
}

// ========================================================
// 初期化
// ========================================================
void KTFramework::Init() {
    /// ウィンドウ初期化
    Keta::Initialize(kWindowTitle, 1280, 720);

    // グローバル変数の読み込み
    GlobalParameter::GetInstance()->LoadFiles();

    // コリジョン
    collisionManager_ = std::make_unique<CollisionManager>();
    collisionManager_->Init();
}

// ========================================================
// 更新処理
// ========================================================
void KTFramework::Update() {
    /// FPS表示
    DisplayFPS();
    /// グローバル変数の更新
    GlobalParameter::GetInstance()->Update();
    /// ゲームシーンの毎フレーム処理
    pSceneManager_->Update();
    /// 当たり判定
    collisionManager_->Update();
}

// ========================================================
// 解放
// ========================================================
void KTFramework::Finalize() {
    // ライブラリの終了
    Keta::Finalize();
}

// ========================================================
// FPS表示
// ========================================================
void KTFramework::DisplayFPS(){

#ifdef _DEBUG
	ImGuiIO& io = ImGui::GetIO();

	// 位置
	ImGui::SetNextWindowPos(ImVec2(1230, 0), ImGuiCond_Always);
	ImGui::SetNextWindowBgAlpha(0.0f); // 背景を完全透明に設定

	// ウィンドウフラグを設定
	ImGui::Begin("FPS Overlay", nullptr,
		ImGuiWindowFlags_NoTitleBar |         // タイトルバーを非表示
		ImGuiWindowFlags_NoResize |          // リサイズを禁止
		ImGuiWindowFlags_NoMove |            // ウィンドウの移動を禁止
		ImGuiWindowFlags_NoScrollbar |       // スクロールバーを非表示
		ImGuiWindowFlags_NoCollapse |        // 折りたたみボタンを非表示
		ImGuiWindowFlags_AlwaysAutoResize |  // 必要なサイズに自動調整
		ImGuiWindowFlags_NoBackground        // 背景を非表示
	);

	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(100, 255, 100, 255));/// 色設定（緑）
	ImGui::Text("%.1f", io.Framerate);
	ImGui::PopStyleColor();

	ImGui::End();
#endif 
}