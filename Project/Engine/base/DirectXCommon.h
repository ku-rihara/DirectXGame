#pragma once

#include<Windows.h>
#include<cstdint>
#include"Matrix4x4.h"
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>
#include<dxgidebug.h>
#include <wrl.h>
#include<format>
#include<chrono>
//class
#include"WinApp.h"




class SrvManager;
class DirectXCommon {
private://メンバ変数

	//ウィンドウズアプリケーション管理
	WinApp* winApp_;

	///===========================================================================
   ///デバイス初期化関連
  ///===========================================================================
	Microsoft::WRL::ComPtr<IDXGIFactory7>dxgiFactory_;
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_;

	///===========================================================================
	///コマンド初期化関連
	///===========================================================================
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator>commandAllocator_;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue>commandQueue_;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;

	///===========================================================================
	///スワップチェーン初期化関連
	///===========================================================================
	Microsoft::WRL::ComPtr<IDXGISwapChain4>swapChain_;
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources_[2] = {};
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};
	uint32_t backBufferWidth_;
	uint32_t backBufferHeight_;

	///===========================================================================
	///RTV関連
	///===========================================================================
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap > rtvDescriptorHeap_;
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[3];//RTVを2つ作るのでディスクリプタを2つ用意

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>dsvDescriptorHeap_;
	Microsoft::WRL::ComPtr<ID3D12Resource>depthStencilResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource>renderTextureResource_;
	D3D12_CPU_DESCRIPTOR_HANDLE renderTextureCPUSrvHandle_;
	D3D12_GPU_DESCRIPTOR_HANDLE renderTextureGPUSrvHandle_;
	D3D12_RESOURCE_STATES renderTextureCurrentState_ = D3D12_RESOURCE_STATE_RENDER_TARGET; 
	D3D12_CLEAR_VALUE clearValue_;
	///===================================
	///フェンス生成関連
	///===================================
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
	HANDLE fenceEvent_;
	uint64_t fenceValue_ = 0;

	///===================================
	///dxCompiler初期化関連
	///===================================
	IDxcUtils* dxcUtils_;
	IDxcCompiler3* dxcCompiler_;
	IDxcIncludeHandler* includeHandler_;

	D3D12_VIEWPORT viewport_{};
	D3D12_RECT scissorRect_{};

	/// DescriptorSize

	uint32_t descriptorSizeRTV_;
	uint32_t descriptorSizeDSV_;

	//バリア
	D3D12_RESOURCE_BARRIER barrier_{};
	UINT backBufferIndex_;


private:

	///==========================================================
	///Private method
	///==========================================================

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
	/// 深度バッファ生成
	/// </summary>
	void CreateDepthBuffer();

	/// <summary>
	/// フェンスの生成
	/// </summary>
	void CreateFence();

	/// <summary>
	/// dxcCompilerの初期化
	/// </summary>
	void dxcCompilerInit();

	Microsoft::WRL::ComPtr <ID3D12Resource> CreateDepthStencilTextureResource(Microsoft::WRL::ComPtr<ID3D12Device>  device, int32_t width, int32_t height);

public:
	HRESULT hr_ = 0;
public:

	///==========================================================
	///public method
	///==========================================================

	//シングルトンインスタンスの取得
	static DirectXCommon* GetInstance();

	void Init(WinApp* win, int32_t backBufferWidth = WinApp::kWindowWidth, int32_t backBufferHeight = WinApp::kWindowHeight);


	void CreateGraphicPipelene();
	void PreDraw();	
	void ClearDepthBuffer();	
	void PostDraw();
	void Finalize();

	//リソースの作成
	Microsoft::WRL::ComPtr < ID3D12Resource>CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device>  device, size_t sizeInBytes);

	/// シェーダーコンパイル
	Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
		//CompilerするShaderファイルパス
		const std::wstring& filePath,
		//Compilerに使用するprofile
		const wchar_t* profile,
		//初期化で生成したものを3つ
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler);

	void commandExecution(Microsoft::WRL::ComPtr <ID3D12Resource>& intermediateResource);

	//DescriptorHeapの作成
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12Device>  device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

public:

	/// RebderTextureの作成
	Microsoft::WRL::ComPtr<ID3D12Resource>
		CreateRenderTextureResource(Microsoft::WRL::ComPtr<ID3D12Device>device, uint32_t width, uint32_t heigth,
			DXGI_FORMAT format, const Vector4& clearColor);

	void CreateRnderSrvHandle();
	void PreRenderTexture();
	void DepthBarrierTransition();
public:
	void PutTransitionBarrier(ID3D12Resource* pResource, D3D12_RESOURCE_STATES Before, D3D12_RESOURCE_STATES After);


	///==========================================================
	///getter method
	///==========================================================
	/// 

	/// dsvDescriptorHeapの取得
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap > descriptorHeap, uint32_t descriptorSize, uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap > descriptorHeap, uint32_t descriptorSize, uint32_t index);

	///TextureRenderのSRVハンドル取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetRenderTextureGPUSrvHandle()const { return renderTextureGPUSrvHandle_; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTextureCPUSrvHandle()const { return renderTextureCPUSrvHandle_; }

	// デバイス初期化関連
	ID3D12Device* GetDevice()const { return device_.Get(); }
	//コマンド
	ID3D12GraphicsCommandList* GetCommandList()const { return commandList_.Get(); }
	//スワップチェーン関連
	DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc()const { return swapChainDesc_; }
	//レンダーターゲットビュー 
	D3D12_RENDER_TARGET_VIEW_DESC GetRtvDesc()const { return rtvDesc_; }

	//dxcCompilerの初期化関連
	IDxcUtils* GetDxcUtils()const { return dxcUtils_; }
	IDxcCompiler3* GetDxcCompiler()const { return dxcCompiler_; }
	IDxcIncludeHandler* GetIncludeHandler()const { return includeHandler_; }

	uint32_t GetDescriptorSizeRTV()const { descriptorSizeRTV_; }
	uint32_t GetDescriptorSizeDSV()const { return descriptorSizeDSV_; }

};

