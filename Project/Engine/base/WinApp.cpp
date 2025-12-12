#include "WinApp.h"

using namespace KetaEngine;
#pragma comment(lib, "winmm.lib")

const wchar_t WinApp::kWindowClassName[] = L"DirectXGame";

#ifdef _DEBUG
#include <imgui_impl_win32.h>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif

// ウィンドウプロシージャ
LRESULT CALLBACK WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
#ifdef _DEBUG
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) {
        return (LRESULT) true;
    }
#endif
    // メッセージに対してゲーム固有の処理を行う
    switch (msg) {
        // ウィンドウが破棄された
    case WM_DESTROY:
        // OSに対して、アプリの終了を伝える
        PostQuitMessage(0);
        return 0;
    }

    // 標準のメッセージ処理を行う
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void WinApp::MakeWindow(const wchar_t* title, int32_t clientWidth, int32_t clientHeight) {
    // COMの初期化
    CoInitializeEx(0, COINIT_MULTITHREADED);
    // システムタイマーの分解能をあげる
    timeBeginPeriod(1);
    // ウィンドウプロシージャ
    wc_.lpfnWndProc = WindowProc;
    // ウィンドウクラス名
    wc_.lpszClassName = kWindowClassName;
    // インスタンスハンドル
    wc_.hInstance = GetModuleHandle(nullptr);
    // カーソル
    wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);

    // ウィンドウクラスを登録する
    RegisterClass(&wc_);

    // ウィンドウサイズを表す構造体にクライアント領域を入れる
    RECT wrc = {0, 0, clientWidth, clientHeight};

    // クライアント領域を元に実際のサイズにwrcを変更してもらう
    AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

    // ウィンドウの生成
    hwnd_ = CreateWindow(
        wc_.lpszClassName, // 利用するクラス名
        title, // タイトルバーの文字
        WS_OVERLAPPEDWINDOW, // 良く見るウィンドウスタイル
        CW_USEDEFAULT, // 表示X座標（Windowsに任せる）
        CW_USEDEFAULT, // 表示Y座標（WindowsOSに任せる）
        wrc.right - wrc.left, // ウィンドウ横幅
        wrc.bottom - wrc.top, // ウィンドウ縦幅
        nullptr, // 親ウィンドウハンドル
        nullptr, // メニューハンドル
        wc_.hInstance, // インスタンスハンドル
        nullptr); // オプション

#ifdef _DEBUG
    debugController_ = nullptr;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController_)))) {
        // デバッグレイヤーを有効化する
        debugController_->EnableDebugLayer();
        // さらにGPU側でもチェックを行えるようにする
        debugController_->SetEnableGPUBasedValidation(TRUE);
    }
#endif
    // ウィンドウを表示する
    ShowWindow(hwnd_, SW_SHOW);
}

int WinApp::ProcessMessage() {
    MSG msg{}; // メッセージ

    // Windowにメッセージが来てたら最優先で処理させる
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { // メッセージがあるか
        TranslateMessage(&msg); // キー入力メッセージの処理
        DispatchMessage(&msg); // ウィンドウプロシージャにメッセージを送る
    }
    // ウィンドウのxボタンが押されるまでループ
    if (msg.message == WM_QUIT) {
        return 1; // 抜ける
    } else {
        return 0; // 続ける
    }
}
