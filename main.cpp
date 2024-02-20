
//class
#include"WinApp.h"
#include"DirectXCommon.h"

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


//windowアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//ウィンドウの生成
	WinApp::MakeWindow(L"CG2",1280,720);


	//スワップチェーンを生成する
	IDXGISwapChain4* swapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

	MSG msg{};
	//ウィンドウのxボタンが押されるまでループ
	while (msg.message != WM_QUIT) {

		//Windowにメッセージが来てたら最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			
		

		}
	}

	return 0;
}

