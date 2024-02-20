#pragma once

#include<Windows.h>
#include<cstdint>


class WinApp {
public://静的メンバ変数

	static const int kWindowWidth = 1280;//横幅
	static const int kWindowHeight = 720;//縦幅

	//ウィンドウクラス名
	static const wchar_t kWindowClassName[];

private://メンバ変数
	WNDCLASS wc_{};
	HWND hwnd_ = nullptr;

public://メンバ関数

	// シングルトンインスタンスの取得
	static WinApp* GetInstance();

	// ウィンドウプロシージャ
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	// ゲームウィンドウの作成
	void MakeWindow(const wchar_t* title = L"DirectXGame", int32_t clientWidth = kWindowWidth, int32_t clientHeight = kWindowHeight);

	// ウィンドウハンドルのゲッター
	HWND GetHwnd() const { return hwnd_; }
};

