#pragma once

#include <cstdint>
#include <d3d12.h>
#include <Windows.h>
#include <wrl.h>
#include <wrl/client.h>

/// <summary>
/// WinAPIアプリケーションクラス
/// </summary>
class WinApp {

public:
    WinApp()  = default;
    ~WinApp() = default;

    /// <summary>
    /// ウィンドウプロシージャ
    /// </summary>
    /// <param name="hwnd">ウィンドウハンドル</param>
    /// <param name="msg">メッセージ</param>
    /// <param name="wParam">wParam</param>
    /// <param name="lParam">lParam</param>
    /// <returns>処理結果</returns>
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

    /// <summary>
    /// ゲームウィンドウの作成
    /// </summary>
    /// <param name="title">ウィンドウタイトル</param>
    /// <param name="clientWidth">クライアント領域の幅</param>
    /// <param name="clientHeight">クライアント領域の高さ</param>
    void MakeWindow(const wchar_t* title = L"DirectXGame", const int32_t& clientWidth = kWindowWidth, const int32_t& clientHeight = kWindowHeight);

    int ProcessMessage(); //< メッセージ処理

public: // 静的メンバ変数
    static const int kWindowWidth  = 1280; //< 横幅
    static const int kWindowHeight = 720; //< 縦幅

    // ウィンドウクラス名
    static const wchar_t kWindowClassName[];

private: // メンバ変数
    WNDCLASS wc_{};
    HWND hwnd_;
    Microsoft::WRL::ComPtr<ID3D12Debug1> debugController_;

public:
    // getter
    const HWND& GetHwnd() const { return hwnd_; }
    const HINSTANCE& GetHInstance() const { return wc_.hInstance; }
    ID3D12Debug1* GetDebugController() const { return debugController_.Get(); }
};