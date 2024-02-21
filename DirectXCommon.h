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

	HRESULT hr_ = 0;

	//デバイス関連
	IDXGIFactory7* dxgiFactory_ = nullptr;
	ID3D12Device* device_ = nullptr;

	//コマンド関連
	ID3D12CommandAllocator* commandAllocator_ = nullptr;
	ID3D12CommandQueue* commandQueue_ = nullptr;
	ID3D12GraphicsCommandList* commandList_ = nullptr;
	
	//スワップチェーン関連
	IDXGISwapChain4* swapChain_ = nullptr;
	int32_t backBufferWidth_ = 0;
	int32_t backBufferHeight_ = 0;

	//レンダーターゲットビュー関連
	ID3D12DescriptorHeap* rtvDescriptorHeap_ = nullptr;
	ID3D12Resource* swapChainResources_[2] = { nullptr };
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];//RTVを2つ作るのでディスクリプタを2つ用意

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

	/// <summary>
    /// レンダーターゲットビューの生成
    /// </summary>
	void CreateRenderTargetView();


public://メンバ関数

	//シングルトンインスタンスの取得
	static DirectXCommon* GetInstance();
	/// 初期化	
	void Init(WinApp* win, int32_t backBufferWidth = WinApp::kWindowWidth, int32_t backBufferHeight = WinApp::kWindowHeight);

	//画面のクリア
	void ScreenClear();

	//コマンドのキック
	void CommandKick();

	//getter
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList_; }

	ID3D12CommandAllocator* GetCommandAllocator()const { return commandAllocator_; }

};
