#pragma once
#include"WinApp.h"
#include"DirectXCommon.h"

class Keta{
public:

	/// システム全体の初期化
	static void Initialize(const char* title, int width = 1280, int height = 720);

	//Windowsのメッセージを処理
	static int ProcessMessage();

	//フレーム開始処理
	static void BeginFrame();

	//フレーム終了処理
	static void EndFrame();
	
	/// システム全体の終了
	static void Finalize();
};

