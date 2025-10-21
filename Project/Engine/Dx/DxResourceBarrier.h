#pragma once
#include <d3d12.h>
#include <unordered_map>
#include <vector>
#include <wrl/client.h>

class DxResourceBarrier {
public:
    DxResourceBarrier()  = default;
    ~DxResourceBarrier() = default;

    // リソースの初期状態を登録
    void RegisterResource(ID3D12Resource* resource, D3D12_RESOURCE_STATES initialState);

    // リソースの登録を解除
    void UnregisterResource(ID3D12Resource* resource);

    // 状態遷移バリアを設定
    void Transition(
        ID3D12GraphicsCommandList* commandList,
        ID3D12Resource* resource,
        D3D12_RESOURCE_STATES afterState,
        UINT subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

    // 強制的に状態遷移バリアを設定
    void TransitionForce(
        ID3D12GraphicsCommandList* commandList,
        ID3D12Resource* resource,
        D3D12_RESOURCE_STATES beforeState,
        D3D12_RESOURCE_STATES afterState,
        UINT subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

    // UAVバリアを設定
    void UAVBarrier(
        ID3D12GraphicsCommandList* commandList,
        ID3D12Resource* resource);

    // エイリアシングバリアを設定
    void AliasingBarrier(
        ID3D12GraphicsCommandList* commandList,
        ID3D12Resource* beforeResource,
        ID3D12Resource* afterResource);

    // 現在の状態を取得
    D3D12_RESOURCE_STATES GetCurrentState(ID3D12Resource* resource) const;

    // 現在の状態を手動で更新
    void UpdateState(ID3D12Resource* resource, D3D12_RESOURCE_STATES newState);

private:
    // リソースの現在の状態を管理
    std::unordered_map<ID3D12Resource*, D3D12_RESOURCE_STATES> resourceStates_;

    // バリアを作成するヘルパー関数
    D3D12_RESOURCE_BARRIER CreateTransitionBarrier(
        ID3D12Resource* resource,
        D3D12_RESOURCE_STATES beforeState,
        D3D12_RESOURCE_STATES afterState,
        UINT subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

    D3D12_RESOURCE_BARRIER CreateUAVBarrier(ID3D12Resource* resource);

    D3D12_RESOURCE_BARRIER CreateAliasingBarrier(
        ID3D12Resource* beforeResource,
        ID3D12Resource* afterResource);
};