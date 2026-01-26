#pragma once

#include <array>
#include <cstdint>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <wrl/client.h>

namespace KetaEngine {

class RtvManager;
class WinApp;

/// <summary>
/// スワップチェーンを管理するクラス
/// </summary>
class DxSwapChain {
public:
    DxSwapChain()  = default;
    ~DxSwapChain() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="dxGiFactory">DXGIファクトリー</param>
    /// <param name="commandQueue">コマンドキュー</param>
    /// <param name="winApp">ウィンドウアプリケーション</param>
    /// <param name="backBufferWidth">バックバッファ幅</param>
    /// <param name="backBufferHeight">バックバッファ高さ</param>
    void Init(
        Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory,
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue,
        WinApp* winApp, int32_t backBufferWidth, int32_t backBufferHeight);

    /// <summary>
    /// レンダーターゲットビュー作成
    /// </summary>
    /// <param name="rtvManager">RTVマネージャー</param>
    void CreateRenderTargetViews(RtvManager* rtvManager);

    /// <summary>
    /// リソース状態更新
    /// </summary>
    /// <param name="index">バッファインデックス</param>
    /// <param name="state">新しい状態</param>
    void UpdateResourceState(const UINT& index, const D3D12_RESOURCE_STATES& state);

    /// <summary>
    /// リソース状態取得
    /// </summary>
    /// <param name="index">バッファインデックス</param>
    /// <returns>リソース状態</returns>
    D3D12_RESOURCE_STATES GetResourceState(const UINT& index) const;
    
    void Present();//< プレゼント
    void WaitForGPU();//< GPU同期
    void Finalize();//< 終了処理

private:
    Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
    Microsoft::WRL::ComPtr<ID3D12Resource> resources_[2];
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
    DXGI_SWAP_CHAIN_DESC1 desc_;
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_;
    HRESULT hr_;

    std::array<D3D12_RESOURCE_STATES, 2> resourceStates_;

public:
    Microsoft::WRL::ComPtr<IDXGISwapChain4> GetSwapChain() const { return swapChain_; }
    Microsoft::WRL::ComPtr<ID3D12Resource> GetSwapChainResource(int index) const {
        return resources_[index];
    }
    UINT GetCurrentBackBufferIndex() const { return swapChain_->GetCurrentBackBufferIndex(); }
    const DXGI_SWAP_CHAIN_DESC1& GetDesc() const { return desc_; }
    const D3D12_RENDER_TARGET_VIEW_DESC& GetRTVDesc() const { return rtvDesc_; }
};

}; // KetaEngine
