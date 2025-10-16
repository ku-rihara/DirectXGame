#pragma once

#include <array>
#include <cstdint>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <wrl/client.h>

class RtvManager;
class WinApp;

class DxSwapChain {
public:
    DxSwapChain()  = default;
    ~DxSwapChain() = default;

    // 初期化
    void Init(
        Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory,
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue,
        WinApp* winApp, const int32_t& backBufferWidth, const int32_t& backBufferHeight);

    // RTV作成
    void CreateRenderTargetViews(RtvManager* rtvManager);
    // プレゼント
    void Present();

    // リソース状態管理
    void UpdateResourceState(const UINT& index, const D3D12_RESOURCE_STATES& state);
    D3D12_RESOURCE_STATES GetResourceState(const UINT& index) const;

    // GPU同期
    void WaitForGPU();
    // 終了処理
    void Finalize();

private:
    Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
    Microsoft::WRL::ComPtr<ID3D12Resource> resources_[2];
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
    DXGI_SWAP_CHAIN_DESC1 desc_;
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_;
    HRESULT hr_;

    // リソース状態の追跡
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