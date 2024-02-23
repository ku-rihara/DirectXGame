#include"Keta.h"

//class

////Function
//#include"Convert.h"

////std
//#include<format>
//
//#include<d3d12.h>
//#include<dxgi1_6.h>
//#include<cassert>
//
//#pragma comment(lib,"d3d12.lib")
//#pragma comment(lib,"dxgi.lib")


const char kWindowTitle[] = "CG2";

//windowアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//ライブラリの初期化
	Keta::Initialize(kWindowTitle, 1280, 720);


	MSG msg{};
	//ウィンドウのxボタンが押されるまでループ
	while (msg.message != WM_QUIT) {

		//Windowにメッセージが来てたら最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			Keta::BeginFrame();

			Keta::EndFrame();
		}
	}

	Keta::Finalize();
	return 0;
}

