#pragma once

#include <cstdint>
#include <d3d12.h>
#include <Windows.h>
#include <wrl.h>
#include <wrl/client.h>

class WinApp {

public:

    WinApp() = default;
    ~WinApp() = default;

    // ウィンドウプロシージャ
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

    // ゲームウィンドウの作成
    void MakeWindow(const wchar_t* title = L"DirectXGame", int32_t clientWidth = kWindowWidth, int32_t clientHeight = kWindowHeight);

    int ProcessMessage();

    // getter
    HWND GetHwnd() const { return hwnd_; }
    HINSTANCE GetHInstaice() const { return wc_.hInstance; }
    ID3D12Debug1* GetDebugController() const { return debugController_.Get(); }

public: // 静的メンバ変数
    static const int kWindowWidth  = 1280; // 横幅
    static const int kWindowHeight = 720; // 縦幅

    // ウィンドウクラス名
    static const wchar_t kWindowClassName[];

private: // メンバ変数
    WNDCLASS wc_{};
    HWND hwnd_;
    Microsoft::WRL::ComPtr<ID3D12Debug1> debugController_;
};
