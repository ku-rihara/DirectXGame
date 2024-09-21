#include<GameScene.h>


const char kWindowTitle[] = "LE2A_11_クリハラ_ケイタ_CG3";

//windowアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	
	Keta::Initialize(kWindowTitle, 1280, 720);

	//ゲームシーン初期化
	GameScene* gameScene = nullptr;

	// ゲームシーンの初期化
	gameScene = new GameScene();
	gameScene->Init();

	//ウィンドウのxボタンが押されるまでループ
	while (Keta::ProcessMessage() == 0) {
		//フレームの開始
		Keta::BeginFrame();
	
		// ゲームシーンの毎フレーム処理
		gameScene->Update();
		// ゲームシーンの描画
		gameScene->Draw();
		
		//フレームの終了
		Keta::EndFrame();
	}

	//ライブラリの終了
	Keta::Finalize();
	
	return 0;

}

