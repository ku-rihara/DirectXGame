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
	int32_t backBufferWidth_ = 0;
	int32_t backBufferHeight_ = 0;
	ID3D12Device* device_ = nullptr;
	HRESULT hr_=0;

public://メンバ関数

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
