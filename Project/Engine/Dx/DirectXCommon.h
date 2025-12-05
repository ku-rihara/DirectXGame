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
// Dx
class DxDevice;
class DxCommand;
class DxSwapChain;
class DxRenderTarget;
class DxCompiler;
class DxDepthBuffer;
class DxResourceBarrier;

enum class ViewType {
    UnorderedAccess,
    ShaderResource,
};

/// <summary>
/// DirectXの共通クラス
/// </summary>
class DirectXCommon {
private:
    ///==========================================================
    /// Private method
    ///==========================================================

    void InitDxClasses();           //< DirectXクラス群の初期化
    void SetupViewportAndScissor(); //< ビューポートとシザー矩形の設定

public:
    HRESULT hr_ = 0;

public:
    ///==========================================================
    /// public method
    ///==========================================================

    static DirectXCommon* GetInstance(); //< シングルトンインスタンスの取得
    ~DirectXCommon() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="win">WinApp</param>
    /// <param name="backBufferWidth">バックバッファ幅</param>
    /// <param name="backBufferHeight">バックバッファ高さ</param>
    void Init(WinApp* win, const int32_t& backBufferWidth = 1280, const int32_t& backBufferHeight = 720);

    /// <summary>
    /// リソースの作成
    /// </summary>
    /// <param name="device">デバイス</param>
    /// <param name="sizeInBytes">サイズ</param>
    /// <param name="viewType">ビュータイプ</param>
    /// <returns>リソース</returns>
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, const size_t& sizeInBytes, const ViewType& viewType = ViewType::ShaderResource);

    /// <summary>
    /// DescriptorHeapの作成
    /// </summary>
    /// <param name="device">デバイス</param>
    /// <param name="heapType">ヒープタイプ</param>
    /// <param name="numDescriptors">ディスクリプタ数</param>
    /// <param name="shaderVisible">シェーダー可視性</param>
    /// <returns>ディスクリプタヒープ</returns>
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> InitializeDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12Device> device,
        const D3D12_DESCRIPTOR_HEAP_TYPE& heapType, const UINT& numDescriptors, const bool& shaderVisible);

    /// <summary>
    /// CPUディスクリプタハンドル取得
    /// </summary>
    /// <param name="descriptorHeap">ディスクリプタヒープ</param>
    /// <param name="descriptorSize">ディスクリプタサイズ</param>
    /// <param name="index">インデックス</param>
    /// <returns>CPUディスクリプタハンドル</returns>
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);

    /// <summary>
    /// GPUディスクリプタハンドル取得
    /// </summary>
    /// <param name="descriptorHeap">ディスクリプタヒープ</param>
    /// <param name="descriptorSize">ディスクリプタサイズ</param>
    /// <param name="index">インデックス</param>
    /// <returns>GPUディスクリプタハンドル</returns>
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);

    void InitRenderingResources(); //< レンダリングリソースの初期化
    void PreDraw();                //< 描画前処理
    void PostDraw();               //< 描画後処理
    void Finalize();               //< 終了処理
    void PreRenderTexture();       //< レンダーテクスチャ描画前処理
    void DepthBarrierTransition(); //< デプスバリア遷移

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
    std::unique_ptr<DxDepthBuffer> depthBuffer_;
    std::unique_ptr<DxResourceBarrier> resourceBarrier_;

    ///===========================================================================
    /// バックバッファ関連
    ///===========================================================================
    uint32_t backBufferWidth_;
    uint32_t backBufferHeight_;

public:
    ///==========================================================
    /// getter method
    ///==========================================================

    // dxポインタ
    DxDevice* GetDxDevice() const { return dxDevice_.get(); }
    DxCommand* GetDxCommand() const { return dxCommand_.get(); }
    DxSwapChain* GetDxSwapChain() const { return dxSwapChain_.get(); }
    DxRenderTarget* GetDxRenderTarget() const { return dxRenderTarget_.get(); }
    DxCompiler* GetDxCompiler() const { return dxCompiler_.get(); }
    DxDepthBuffer* GetDepthBuffer() const { return depthBuffer_.get(); }
    DxResourceBarrier* GetResourceBarrier() const { return resourceBarrier_.get(); }

    Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() const;
    ID3D12GraphicsCommandList* GetCommandList() const;
};