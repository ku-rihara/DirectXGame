#pragma once
#include "DxDepthBuffer.h"
#include "Vector4.h"
#include <cstdint>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <memory>
#include <wrl/client.h>

class RtvManager;
class SrvManager;
class DxCommand;
class DxSwapChain;
class DxDepthBuffer;
class DxResourceBarrier;

/// <summary>
/// レンダーターゲットを管理するクラス
/// </summary>
class DxRenderTarget {
public:
    DxRenderTarget()  = default;
    ~DxRenderTarget() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="device">D3D12デバイス</param>
    /// <param name="width">レンダーターゲット幅</param>
    /// <param name="height">レンダーターゲット高さ</param>
    void Init(Microsoft::WRL::ComPtr<ID3D12Device> device, const uint32_t& width, const uint32_t& height);

   
    void PreRenderTexture();          //< レンダーテクスチャへの描画準備
    void PreDraw();                   //< バックバッファへの描画準備
    void PostDrawTransitionBarrier(); //< 描画終了後のバリア設定
    void SetupViewportAndScissor();   //< ビューポートとシザー矩形設定
    void Finalize();                  //< 終了処理

private:
    
    void CreateRenderTextureRTV();//< レンダーテクスチャRTV作成
    void CreateRenderTextureSRV();//< レンダーテクスチャSRV作成

    /// <summary>
    /// レンダーテクスチャリソース作成
    /// </summary>
    /// <param name="device">D3D12デバイス</param>
    /// <param name="width">テクスチャ幅</param>
    /// <param name="height">テクスチャ高さ</param>
    /// <param name="format">フォーマット</param>
    /// <param name="clearColor">クリアカラー</param>
    /// <returns>レンダーテクスチャリソース</returns>
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateRenderTextureResource(
        Microsoft::WRL::ComPtr<ID3D12Device> device, const uint32_t& width, const uint32_t& height,
        DXGI_FORMAT format, const Vector4& clearColor);

private:
    RtvManager* rtvManager_               = nullptr;
    SrvManager* srvManager_               = nullptr;
    DxCommand* dxCommand_                 = nullptr;
    DxSwapChain* dxSwapChain_             = nullptr;
    DxDepthBuffer* depthBuffer_           = nullptr;
    DxResourceBarrier* dxResourceBarrier_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> renderTextureResource_;
    D3D12_GPU_DESCRIPTOR_HANDLE renderTextureGPUSrvHandle_;
    D3D12_CPU_DESCRIPTOR_HANDLE renderTextureCPUSrvHandle_;
    D3D12_CLEAR_VALUE clearValue_;

    UINT backBufferIndex_;
    D3D12_VIEWPORT viewport_{};
    D3D12_RECT scissorRect_{};
    uint32_t backBufferWidth_;
    uint32_t backBufferHeight_;
    Vector4 clearColor_;
    uint32_t renderTextureRtvIndex_;

public:
    Microsoft::WRL::ComPtr<ID3D12Resource> GetRenderTextureResource() const { return renderTextureResource_; }
    const D3D12_GPU_DESCRIPTOR_HANDLE& GetRenderTextureGPUSrvHandle() const { return renderTextureGPUSrvHandle_; }
    const D3D12_CPU_DESCRIPTOR_HANDLE& GetRenderTextureCPUSrvHandle() const { return renderTextureCPUSrvHandle_; }
    const D3D12_CLEAR_VALUE& GetClearValue() const { return clearValue_; }

    /// <summary>
    /// 使用するクラスを設定
    /// </summary>
    /// <param name="depthBuffer">深度バッファ</param>
    /// <param name="rtvManager">RTVマネージャー</param>
    /// <param name="srvManager">SRVマネージャー</param>
    /// <param name="dxCommand">コマンド</param>
    /// <param name="dxSwapChain">スワップチェーン</param>
    /// <param name="resourceBarrier">リソースバリア</param>
    void SetUseClasses(DxDepthBuffer* depthBuffer, RtvManager* rtvManager, SrvManager* srvManager,
        DxCommand* dxCommand, DxSwapChain* dxSwapChain, DxResourceBarrier* resourceBarrier);
};