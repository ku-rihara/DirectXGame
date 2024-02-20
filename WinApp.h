#pragma once

#include<Windows.h>
#include<cstdint>


class WinApp{
public://静的メンバ変数
	static const int kWindowWidth = 1280;//横幅
	static const int kWindowHeight = 720;//縦幅

	//ウィンドウクラス名
	static const wchar_t kWindowClassName[];
	
public:
	
	static WinApp* GetInstance();

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	 void MakeWindow(const wchar_t* title = L"DirectXGame",int32_t clientWidth = kWindowWidth, int32_t clientHeight = kWindowHeight);

private://メンバ変数
	WNDCLASS wc_{};
	
};

