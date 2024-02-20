#pragma once

#include"WinApp.h"
#include"DirectXCommon.h"
class Keta{
public:

	/// <summary>
	/// システム全体の初期化
	/// <param name="title">タイトルバー</param>
	/// <param name="width">ウィンドウ（クライアント領域）の幅</param>
	/// <param name="height">ウィンドウ（クライアント領域）の高さ</param>
	/// </summary>
	static void Initialize(const char* title, int width = 1280, int height = 720);



};

