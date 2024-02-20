#pragma once

#include<Windows.h>
#include<cstdint>
#include<d3d12.h>
#include<dxgi1_6.h>


//class
#include"WinApp.h"

class DirectXCommon {
private://メンバ変数

	//ウィンドウズアプリケーション管理
	WinApp* winApp_;

	//デバイス、コマンド関連
	IDXGIFactory7* dxgiFactory_ = nullptr;
	ID3D12Device* device_ = nullptr;
	ID3D12CommandQueue* commandQueue = nullptr;
	int32_t backBufferWidth_ = 0;
	int32_t backBufferHeight_ = 0;
	HRESULT hr_=0;

	//スワップチェーン関連
	IDXGISwapChain4* swapChain = nullptr;

private://メンバ関数

	/// <summary>
	/// DXGIデバイス初期化
	/// </summary>
		void DXGIDeviceInit();

	/// <summary>
	/// コマンド関連初期化
	/// </summary>
		void CommandInit();

	/// <summary>
	/// スワップチェーンの生成
	/// </summary>
		void CreateSwapChain();


public://メンバ関数

	//シングルトンインスタンスの取得
	static DirectXCommon* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init(WinApp* win, int32_t backBufferWidth = WinApp::kWindowWidth, int32_t backBufferHeight = WinApp::kWindowHeight);

};
