#include<Scene/GameScene.h>
#include"GrobalParamater/GlobalParameter.h"
#include "Frame/Frame.h"
#include"Colider/CollisionManager.h"

const char kWindowTitle[] = "LE2A_11_クリハラ_ケイタ_CG3";

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
	Frame::Init();

	//ウィンドウのxボタンが押されるまでループ
	while (Keta::ProcessMessage() == 0) {
		/// フレームの開始
		Keta::BeginFrame();
		/// ===================================================
		///更新
		/// ===================================================
		/// deltaTimeの更新
		Frame::Update();
		/// グローバル変数の更新
		GlobalParameter::GetInstance()->Update();
		/// ゲームシーンの毎フレーム処理
		gameScene->Update();
		///当たり判定
		collisionManager->Update();
		/// ===================================================
		/// 描画
		/// ===================================================
			/// ゲームシーンの描画
		gameScene->Draw();
		///コリジョン描画
		/*collisionManager->Draw(gameScene->GetViewProjection());*/
		/// フレームの終了
		Keta::EndFrame();
	}
	delete gameScene;
	//ライブラリの終了
	Keta::Finalize();
	return 0;

}

