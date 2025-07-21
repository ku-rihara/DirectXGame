#pragma once
#include <d3d12.h>
#include <wrl.h>

class DxCommand {
public:
    DxCommand()  = default;
    ~DxCommand() = default;

    // 初期化
    void Init(Microsoft::WRL::ComPtr<ID3D12Device> device);

    // コマンド実行
    void ExecuteCommand();

    // コマンドリストリセット
    void ResetCommand();

    // フェンス待機
    void WaitForGPU();

    // 終了処理
    void Finalize();

private:
    void CreateCommandQueue(Microsoft::WRL::ComPtr<ID3D12Device> device);
    void CreateCommandAllocator(Microsoft::WRL::ComPtr<ID3D12Device> device);
    void CreateCommandList(Microsoft::WRL::ComPtr<ID3D12Device> device);
    void CreateFence(Microsoft::WRL::ComPtr<ID3D12Device> device);

    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
    Microsoft::WRL::ComPtr<ID3D12Fence> fence_;

    UINT64 fenceValue_;
    HANDLE fenceEvent_;
    HRESULT hr_;

public:
  
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetCommandQueue() const { return commandQueue_; }
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetCommandList() const { return commandList_; }
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> GetCommandAllocator() const { return commandAllocator_; }
};