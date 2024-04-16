#include"Keta.h"


const char kWindowTitle[] = "CG2";

//windowアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//ライブラリの初期化
	Keta::Initialize(kWindowTitle, 1280, 720);

	//ウィンドウのxボタンが押されるまでループ
	while (Keta::ProcessMessage() == 0) {
		//フレームの開始
		Keta::BeginFrame();

		//フレームの終了
		Keta::EndFrame();
	}

	//ライブラリの終了
	Keta::Finalize();
	return 0;
}

