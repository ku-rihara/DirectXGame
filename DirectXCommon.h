#pragma once

#include<Windows.h>
#include<cstdint>
#include"Matrix4x4.h"

#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>
#include<dxgidebug.h>

//class
#include"WinApp.h"

class DirectXCommon {
private://メンバ変数

	//ウィンドウズアプリケーション管理
	WinApp* winApp_;


	//デバイス初期化関連
	IDXGIFactory7* dxgiFactory_;
	ID3D12Device* device_;
	IDXGIAdapter4* useAdapter_;

	//コマンド初期化関連
	ID3D12CommandAllocator* commandAllocator_;
	ID3D12CommandQueue* commandQueue_;
	ID3D12GraphicsCommandList* commandList_;
	
	//スワップチェーン関連
	IDXGISwapChain4* swapChain_;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};
	ID3D12Resource* swapChainResources_[2] = {};
	int32_t backBufferWidth_ = 0;
	int32_t backBufferHeight_ = 0;

	//レンダーターゲットビュー関連
	ID3D12DescriptorHeap* rtvDescriptorHeap_;
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];//RTVを2つ作るのでディスクリプタを2つ用意

	//フェンス生成関連
	ID3D12Fence* fence_;
	HANDLE fenceEvent_;
	uint64_t fenceValue_ = 0;

	//dxcCompilerの初期化関連
	IDxcUtils* dxcUtils_;
	IDxcCompiler3* dxcCompiler_;
	IDxcIncludeHandler* includeHandler_;

	//グラフィックパイプライン関連
	ID3D12RootSignature* rootSignature_;
	ID3DBlob* signatureBlob_;
	ID3DBlob* errorBlob_;
	D3D12_VIEWPORT viewport_{};
	D3D12_RECT scissorRect_{};
	IDxcBlob* vertexShaderBlob_;
	IDxcBlob* pixelShaderBlob_;
	ID3D12PipelineState* graphicsPipelineState_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	ID3D12Resource* vertexResource_;

	//Resource作成
	ID3D12Resource* materialResource_;
	ID3D12Resource* wvpResouce_;
	Matrix4x4* wvpDate_;
	
	//バリア
	D3D12_RESOURCE_BARRIER barrier_{};

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

	/// <summary>
	/// フェンスの生成
	/// </summary>
	void CreateFence();
	
	//リソースの作成
	ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);
	
	/// <summary>
	/// dxcCompilerの初期化
	/// </summary>
	void dxcCompilerInit();

public://メンバ関数
	HRESULT hr_ = 0;

	//シングルトンインスタンスの取得
	static DirectXCommon* GetInstance();
	/// 初期化	
	void Init(WinApp* win, int32_t backBufferWidth = WinApp::kWindowWidth, int32_t backBufferHeight = WinApp::kWindowHeight);

	//レンダリングパイプライン
	void CreateGraphicPipelene();
	
	//画面のクリア
	void ScreenClear();

	//コマンドのキック
	void CommandKick();

	//リソーススリークチェック
	void ResourceLeakCheck();

	//オブジェクトのリリース
	void ReleaseObject();

	//DescriptorHeapの作成
	ID3D12DescriptorHeap* CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

	
	//getter
	// デバイス初期化関連
	ID3D12Device* GetDevice()const { return device_; }
	//スワップチェーン関連
	DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc()const { return swapChainDesc_; }
	//レンダーターゲットビュー 
	D3D12_RENDER_TARGET_VIEW_DESC GetRtvDesc()const { return rtvDesc_; }
	
	//dxcCompilerの初期化関連
	IDxcUtils* GetDxcUtils()const { return dxcUtils_; }
	IDxcCompiler3* GetDxcCompiler()const { return dxcCompiler_; }
	IDxcIncludeHandler* GetIncludeHandler()const { return includeHandler_; }
	
	//setter
	void SetFenceValueIncrement() { this->fenceValue_++; }
	void SetwvpDate(Matrix4x4 date) { *this->wvpDate_=date; }
	
};
