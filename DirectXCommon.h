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
//struct
#include"TransformationMatrix.h"
#include"DirectionalLight.h"

class DirectXCommon {
private://メンバ変数

	//ウィンドウズアプリケーション管理
	WinApp* winApp_;

	//Init------------------------------------------------------------------------------------------------------
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
	uint32_t backBufferWidth_;
	uint32_t backBufferHeight_;

	//レンダーターゲットビュー関連
	ID3D12DescriptorHeap* rtvDescriptorHeap_;
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];//RTVを2つ作るのでディスクリプタを2つ用意

	ID3D12DescriptorHeap* dsvDescriptorHeap_;
	ID3D12Resource* depthStencilResource_;
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};

	//フェンス生成関連
	ID3D12Fence* fence_;
	HANDLE fenceEvent_;
	uint64_t fenceValue_ = 0;

	//dxcCompilerの初期化関連
	IDxcUtils* dxcUtils_;
	IDxcCompiler3* dxcCompiler_;
	IDxcIncludeHandler* includeHandler_;
	//Init------------------------------------------------------------------------------------------------------

	//グラフィックパイプライン関連
	ID3D12RootSignature* rootSignature_;
	ID3DBlob* signatureBlob_;
	ID3DBlob* errorBlob_;
	D3D12_VIEWPORT viewport_{};
	D3D12_RECT scissorRect_{};
	IDxcBlob* vertexShaderBlob_;
	IDxcBlob* pixelShaderBlob_;
	ID3D12PipelineState* graphicsPipelineState_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite_;

	//球
	const uint32_t kSubdivision_ = 16;//分割数
	const uint32_t  shpereVertexNum_ = 1536;
	//Resource
	ID3D12Resource* vertexResource_;
	ID3D12Resource* wvpResouce_;
	ID3D12Resource* vertexResourceSprite_;
	ID3D12Resource* transformationMatrixResourceSprite_;
	TransformationMatrix* wvpDate_;
	TransformationMatrix* transformationMatrixDataSprite_;

	//Material
	ID3D12Resource* materialResource_;
	ID3D12Resource* directionalLightResource_;
	DirectionalLight* directionalLightData_;
	ID3D12Resource* directionalLightResourceSprite_;
	ID3D12Resource* materialResourceSprite_;

	//DescriptorSize
	uint32_t descriptorSizeSRV_;
	uint32_t descriptorSizeRTV_;
	uint32_t descriptorSizeDSV_;

	//バリア
	D3D12_RESOURCE_BARRIER barrier_{};
	UINT backBufferIndex_;

	//後に消すかも
	bool useMonsterBall=true;
	

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

	ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height);

public://メンバ関数
	HRESULT hr_ = 0;

	//シングルトンインスタンスの取得
	static DirectXCommon* GetInstance();
	/// <summary>
	/// 初期化
	/// </summary>	
	void Init(WinApp* win, int32_t backBufferWidth = WinApp::kWindowWidth, int32_t backBufferHeight = WinApp::kWindowHeight);

	/// <summary>
	//レンダリングパイプライン
	/// </summary>	
	void CreateGraphicPipelene();
	
	/// <summary>
	//画面のクリア
	/// </summary>	
	void ScreenClear();

	/// <summary>
	/// 深度バッファのクリア
	/// </summary>
	void ClearDepthBuffer();

	/// <summary>
	//コマンドのキック
	/// </summary>	
	void CommandKick();

	/// <summary>
	//リソーススリークチェック
	/// </summary>	
	void ResourceLeakCheck();

	/// <summary>
	//オブジェクトのリリース
	/// </summary>	
	void ReleaseObject();

	//リソースの作成
	ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

	void commandExecution();

	//DescriptorHeapの作成
	ID3D12DescriptorHeap* CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

	
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

	uint32_t GetDescriptorSizeSRV()const { return descriptorSizeSRV_; }
	uint32_t GetDescriptorSizeRTV()const { descriptorSizeRTV_; }
	uint32_t GetDescriptorSizeDSV()const { return descriptorSizeDSV_; }
	
	//setter
	void SetwvpDate(Matrix4x4 date) { this->wvpDate_->WVP = date; }
	void SetWorldMatrixDate(Matrix4x4 date) { wvpDate_->World = date; }
	void SetTransformationMatrixDataSprite(Matrix4x4 date) { this->transformationMatrixDataSprite_->WVP = date; }
	void SetWorldMatrixDataSprite(Matrix4x4 date) { this->transformationMatrixDataSprite_->World = date; }

};
