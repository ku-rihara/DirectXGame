#pragma once

#include "Matrix4x4.h"
#include <chrono>
#include <cstdint>
#include <d3d12.h>
#include <dxcapi.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <format>
#include <Windows.h>
#include <wrl.h>
// class
#include "base/WinApp.h"

class ImGuiManager;
class TextureManager;
class SrvManager;
class RtvManager;
class DirectXCommon {
private:
    ImGuiManager* imguiManager_     = nullptr;
    TextureManager* textureManager_ = nullptr;
    SrvManager* srvManager_         = nullptr;
    RtvManager* rtvManager_         = nullptr;
    WinApp* winApp_                 = nullptr;

private:
    ///===========================================================================
    /// デバイス関連
    ///===========================================================================
    Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
    Microsoft::WRL::ComPtr<ID3D12Device> device_;
    Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_;

    ///===========================================================================
    /// コマンド関連
    ///===========================================================================
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;

    ///===========================================================================
    /// スワップチェーン関連
    ///===========================================================================
    Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
    Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources_[2] = {};
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};
    uint32_t backBufferWidth_;
    uint32_t backBufferHeight_;

    ///===========================================================================
    /// RTV関連
    ///===========================================================================
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_;
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[3]; // RTVを2つ作るのでディスクリプタを2つ用意

    // Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>dsvDescriptorHeap_;
    Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> renderTextureResource_;
    D3D12_CPU_DESCRIPTOR_HANDLE renderTextureCPUSrvHandle_;
    D3D12_GPU_DESCRIPTOR_HANDLE renderTextureGPUSrvHandle_;
    D3D12_RESOURCE_STATES renderTextureCurrentState_ = D3D12_RESOURCE_STATE_RENDER_TARGET;
    D3D12_CLEAR_VALUE clearValue_;
    ///===================================
    /// フェンス生成関連
    ///===================================
    Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
    HANDLE fenceEvent_;
    uint64_t fenceValue_ = 0;

    ///===================================
    /// dxCompiler初期化関連
    ///===================================
    IDxcUtils* dxcUtils_;
    IDxcCompiler3* dxcCompiler_;
    IDxcIncludeHandler* includeHandler_;

    D3D12_VIEWPORT viewport_{};
    D3D12_RECT scissorRect_{};

    /// DescriptorSize

    uint32_t descriptorSizeDSV_;

    // バリア
    D3D12_RESOURCE_BARRIER barrier_{};
    UINT backBufferIndex_;

private:
    ///==========================================================
    /// Private method
    ///==========================================================
    ///
    /// DXGIデバイス初期化
    void DXGIDeviceInit();
    /// コマンド関連初期化
    void CommandInit();
    /// スワップチェーンの生成
    void CreateSwapChain();
    /// レンダーターゲットビューの生成
    void CreateRenderTargetView();
    /// 深度バッファ生成
    void CreateDepthBuffer();
    /// フェンスの生成
    void CreateFence();
    /// dxcCompilerの初期化
    void dxcCompilerInit();

    Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height);

public:
    HRESULT hr_ = 0;

public:
    ///==========================================================
    /// public method
    ///==========================================================

    // シングルトンインスタンスの取得
    static DirectXCommon* GetInstance();

    DirectXCommon()  = default;
    ~DirectXCommon() = default;

    void Init(WinApp* win, int32_t backBufferWidth = WinApp::kWindowWidth, int32_t backBufferHeight = WinApp::kWindowHeight);
    void InitRenderingResources();

    void CreateGraphicPipelene();
    void PreDraw();
    void ClearDepthBuffer();
    void PostDraw();
    void Finalize();

    // リソースの作成
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInBytes);

    /// シェーダーコンパイル
    Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
        // CompilerするShaderファイルパス
        const std::wstring& filePath,
        // Compilerに使用するprofile
        const wchar_t* profile,
        // 初期化で生成したものを3つ
        IDxcUtils* dxcUtils,
        IDxcCompiler3* dxcCompiler,
        IDxcIncludeHandler* includeHandler);

    void commandExecution(Microsoft::WRL::ComPtr<ID3D12Resource>& intermediateResource);

    // DescriptorHeapの作成
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> InitializeDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

public:
    /// RebderTextureの作成
    Microsoft::WRL::ComPtr<ID3D12Resource>
    CreateRenderTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, uint32_t width, uint32_t heigth,
        DXGI_FORMAT format, const Vector4& clearColor);

    void CreateRnderSrvHandle();
    void PreRenderTexture();
    void DepthBarrierTransition();

public:
    void PutTransitionBarrier(ID3D12Resource* pResource, D3D12_RESOURCE_STATES Before, D3D12_RESOURCE_STATES After);

    ///==========================================================
    /// getter method
    ///==========================================================
    ///

    /// dsvDescriptorHeapの取得
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);

    /// TextureRenderのSRVハンドル取得
    D3D12_GPU_DESCRIPTOR_HANDLE GetRenderTextureGPUSrvHandle() const { return renderTextureGPUSrvHandle_; }
    D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTextureCPUSrvHandle() const { return renderTextureCPUSrvHandle_; }

    ID3D12Device* GetDevice() const { return device_.Get(); }
    ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); }
    DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() const { return swapChainDesc_; }
    D3D12_RENDER_TARGET_VIEW_DESC GetRtvDesc() const { return rtvDesc_; }
    IDxcUtils* GetDxcUtils() const { return dxcUtils_; }
    IDxcCompiler3* GetDxcCompiler() const { return dxcCompiler_; }
    IDxcIncludeHandler* GetIncludeHandler() const { return includeHandler_; }
    uint32_t GetDescriptorSizeDSV() const { return descriptorSizeDSV_; }
};
