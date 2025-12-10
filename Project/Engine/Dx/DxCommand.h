#pragma once
#include <d3d12.h>
#include <wrl/client.h>

/// <summary>
/// コマンドリストを管理するクラス
/// </summary>
namespace KetaEngine {

class DxCommand {
public:
    DxCommand()  = default;
    ~DxCommand() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="device">D3D12デバイス</param>
    void Init(Microsoft::WRL::ComPtr<ID3D12Device> device);

    void ExecuteCommand(); //< コマンド実行
    void ResetCommand(); //< コマンドリストリセット
    void WaitForGPU(); //< フェンス待機
    void Finalize(); //< 終了処理

private:
    /// <summary>
    /// コマンドキュー作成
    /// </summary>
    /// <param name="device">D3D12デバイス</param>
    void CreateCommandQueue(Microsoft::WRL::ComPtr<ID3D12Device> device);

    /// <summary>
    /// コマンドアロケータ作成
    /// </summary>
    /// <param name="device">D3D12デバイス</param>
    void CreateCommandAllocator(Microsoft::WRL::ComPtr<ID3D12Device> device);

    /// <summary>
    /// コマンドリスト作成
    /// </summary>
    /// <param name="device">D3D12デバイス</param>
    void CreateCommandList(Microsoft::WRL::ComPtr<ID3D12Device> device);

    /// <summary>
    /// フェンス作成
    /// </summary>
    /// <param name="device">D3D12デバイス</param>
    void CreateFence(Microsoft::WRL::ComPtr<ID3D12Device> device);

private:
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
    Microsoft::WRL::ComPtr<ID3D12Fence> fence_;

    UINT64 fenceValue_;
    HANDLE fenceEvent_;
    HRESULT hr_;

public:
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetCommandQueue() const { return commandQueue_; }
    ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); }
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> GetCommandAllocator() const { return commandAllocator_; }
};

}; // KetaEngine
