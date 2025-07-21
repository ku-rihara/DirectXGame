#pragma once


#include <cstdint>
#include <d3d12.h>
#include <memory>
#include <Windows.h>
#include <wrl/client.h>

// 前方宣言
class ImGuiManager;
class TextureManager;
class SrvManager;
class RtvManager;
class WinApp;
class DxDevice;
class DxCommand;
class DxSwapChain;
class DxRenderTarget;
class DxCompiler;

class DirectXCommon {
private:
 

    ImGuiManager* imguiManager_     = nullptr;
    TextureManager* textureManager_ = nullptr;
    SrvManager* srvManager_         = nullptr;
    RtvManager* rtvManager_         = nullptr;
    WinApp* winApp_                 = nullptr;

    // DirectXクラス群
    std::unique_ptr<DxDevice> dxDevice_;
    std::unique_ptr<DxCommand> dxCommand_;
    std::unique_ptr<DxSwapChain> dxSwapChain_;
    std::unique_ptr<DxRenderTarget> dxRenderTarget_;
    std::unique_ptr<DxCompiler> dxCompiler_;

    ///===========================================================================
    /// バックバッファ関連
    ///===========================================================================
    uint32_t backBufferWidth_;
    uint32_t backBufferHeight_;

    ///===========================================================================
    /// ビューポート・シザー矩形
    ///===========================================================================
    D3D12_VIEWPORT viewport_{};
    D3D12_RECT scissorRect_{};

private:
    ///==========================================================
    /// Private method
    ///==========================================================

    /// DirectXクラス群の初期化
    void InitDxClasses();
    /// ビューポートとシザー矩形の設定
    void SetupViewportAndScissor();

public:
    HRESULT hr_ = 0;

public:
    ///==========================================================
    /// public method
    ///==========================================================

    // シングルトンインスタンスの取得
    static DirectXCommon* GetInstance();
    ~DirectXCommon() = default;

    void Init(WinApp* win, int32_t backBufferWidth = 1280, int32_t backBufferHeight = 720);
    void InitRenderingResources();

    void CreateGraphicPipeline();
    void PreDraw();
    void ClearDepthBuffer();
    void PostDraw();
    void Finalize();

    // リソースの作成
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInBytes);

    void commandExecution(Microsoft::WRL::ComPtr<ID3D12Resource>& intermediateResource);

    // DescriptorHeapの作成
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> InitializeDescriptorHeap(
        Microsoft::WRL::ComPtr<ID3D12Device> device,
        D3D12_DESCRIPTOR_HEAP_TYPE heapType,
        UINT numDescriptors,
        bool shaderVisible);

public:
    /// レンダーテクスチャ関連
    void PreRenderTexture();
    void DepthBarrierTransition();

public:
    void PutTransitionBarrier(ID3D12Resource* pResource, D3D12_RESOURCE_STATES Before, D3D12_RESOURCE_STATES After);

    ///==========================================================
    /// getter method
    ///==========================================================

    /// ディスクリプタハンドル取得
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap,
        uint32_t descriptorSize,
        uint32_t index);

    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap,
        uint32_t descriptorSize,
        uint32_t index);

    DxDevice* GetDxDevice() const { return dxDevice_.get(); }
    DxCommand* GetDxCommand() const { return dxCommand_.get(); }
    DxSwapChain* GetDxSwapChain() const { return dxSwapChain_.get(); }
    DxRenderTarget* GetDxRenderTarget() const { return dxRenderTarget_.get(); }
    DxCompiler* GetDxCompiler() const { return dxCompiler_.get(); }

 
    Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() const;
    ID3D12GraphicsCommandList* GetCommandList() const;

    D3D12_GPU_DESCRIPTOR_HANDLE GetRenderTextureGPUSrvHandle() const;
    D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTextureCPUSrvHandle() const;
};