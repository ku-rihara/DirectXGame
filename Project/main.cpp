#include<Scene/GameScene.h>
#include"GrobalParamater/GlobalParameter.h"
#include "Frame/Frame.h"
#include"Colider/CollisionManager.h"

const char kWindowTitle[] = "LE2A_11_クリハラ_ケイタ_CG3";

void DisplayFPS()
{
#ifdef _DEBUG
	ImGuiIO& io = ImGui::GetIO();

	// ウィンドウ固定
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

	// 文字色を緑に設定
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(100, 255, 100, 255));
	ImGui::Text("%.1f", io.Framerate);
	ImGui::PopStyleColor();

	ImGui::End();
#endif // _DEBUG
}


//ゲームシーン初期化
GameScene* gameScene = nullptr;

//windowアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	Keta::Initialize(kWindowTitle, 1280, 720);
	//グローバル変数の読み込み
	GlobalParameter::GetInstance()->LoadFiles();
	// ゲームシーンの生成
	gameScene = new GameScene();
	gameScene->Init();
	/// コリジョンマネージャー
	CollisionManager* collisionManager = new CollisionManager();
	collisionManager->Init();

	/// 時間の初期化
	/*Frame::Init();*/

	//ウィンドウのxボタンが押されるまでループ
	while (Keta::ProcessMessage() == 0) {
		/// フレームの開始
		Keta::BeginFrame();
		/// ===================================================
		///更新
		/// ===================================================
		/// deltaTimeの更新
	/*	Frame::Update();*/
		/// グローバル変数の更新
		GlobalParameter::GetInstance()->Update();
		/// ゲームシーンの毎フレーム処理
		gameScene->Update();
		///当たり判定
		collisionManager->Update();
		/// ===================================================
		/// 描画
		/// ===================================================
		ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();
		///*******
		/// モデル
		///*******
		Model::PreDraw(commandList);
		/// ゲームシーン描画
		gameScene->ModelDraw();
		/// コリジョン描画
		collisionManager->Draw(gameScene->GetViewProjection());
		///*******
		/// パーティクル
		///*******
		Model::PreDrawParticle(commandList);
		/// ゲームシーン描画
		gameScene->ParticleDraw();
		///*******
		/// スプライト
		///*******
		Sprite::PreDraw(commandList);
		gameScene->SpriteDraw();

		DisplayFPS();

			/// フレームの終了
		Keta::EndFrame();
	}
	delete gameScene;
	//ライブラリの終了
	Keta::Finalize();
	return 0;

}

