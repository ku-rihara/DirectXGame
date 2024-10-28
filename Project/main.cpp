#include<Scene/GameScene.h>
#include"Frame/Frame.h"

const char kWindowTitle[] = "LE2A_11_クリハラ_ケイタ_CG3";

//ゲームシーン初期化
GameScene* gameScene = nullptr;

//windowアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	Keta::Initialize(kWindowTitle, 1280, 720);

	// ゲームシーンの生成
	gameScene = new GameScene();
	gameScene->Init();

	//Frame初期化
	Frame::Init();

	//ウィンドウのxボタンが押されるまでループ
	while (Keta::ProcessMessage() == 0) {
		//フレームの開始
		Keta::BeginFrame();
		//deltatime更新
		Frame::Update();
		// ゲームシーンの毎フレーム処理
		gameScene->Update();
		// ゲームシーンの描画
		gameScene->Draw();

		//フレームの終了
		Keta::EndFrame();
	}
	delete gameScene;
	//ライブラリの終了
	Keta::Finalize();
	return 0;

}

