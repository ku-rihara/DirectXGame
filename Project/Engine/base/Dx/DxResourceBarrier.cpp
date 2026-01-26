#include "DxResourceBarrier.h"

using namespace KetaEngine;
#include <cassert>

///==========================================================
/// リソースの初期状態を登録
///==========================================================
void DxResourceBarrier::RegisterResource(ID3D12Resource* resource, D3D12_RESOURCE_STATES initialState) {
    assert(resource != nullptr);
    resourceStates_[resource] = initialState;
}

///==========================================================
/// リソースの登録を解除
///==========================================================
void DxResourceBarrier::UnregisterResource(ID3D12Resource* resource) {
    resourceStates_.erase(resource);
}

///==========================================================
/// 状態遷移バリア
///==========================================================
void DxResourceBarrier::Transition(
    ID3D12GraphicsCommandList* commandList,
    ID3D12Resource* resource,
    D3D12_RESOURCE_STATES afterState,
    UINT subresource) {

    assert(commandList != nullptr);
    assert(resource != nullptr);

    // リソースの現在の状態を取得
    auto it = resourceStates_.find(resource);
    if (it == resourceStates_.end()) {
        // 未登録の場合は警告を出して処理をスキップ
        OutputDebugStringA("Warning: Resource not registered in DxResourceBarrier.\n");
        return;
    }

    D3D12_RESOURCE_STATES beforeState = it->second;

    // 状態が同じ場合はバリアを設定しない
    if (beforeState == afterState) {
        return;
    }

    // バリアを作成して実行
    D3D12_RESOURCE_BARRIER barrier = CreateTransitionBarrier(resource, beforeState, afterState, subresource);
    commandList->ResourceBarrier(1, &barrier);

    // 状態を更新
    resourceStates_[resource] = afterState;
}

///==========================================================
/// 強制的に状態遷移バリアを設定
///==========================================================
void DxResourceBarrier::TransitionForce(
    ID3D12GraphicsCommandList* commandList,
    ID3D12Resource* resource,
    D3D12_RESOURCE_STATES beforeState,
    D3D12_RESOURCE_STATES afterState,
    UINT subresource) {

    assert(commandList != nullptr);
    assert(resource != nullptr);

    // 状態が同じ場合はバリアを設定しない
    if (beforeState == afterState) {
        return;
    }

    // バリアを作成して実行
    D3D12_RESOURCE_BARRIER barrier = CreateTransitionBarrier(resource, beforeState, afterState, subresource);
    commandList->ResourceBarrier(1, &barrier);

    // 状態を更新
    resourceStates_[resource] = afterState;
}

///==========================================================
/// UAVバリア
///==========================================================
void DxResourceBarrier::UAVBarrier(
    ID3D12GraphicsCommandList* commandList,
    ID3D12Resource* resource) {

    assert(commandList != nullptr);
    assert(resource != nullptr);

    D3D12_RESOURCE_BARRIER barrier = CreateUAVBarrier(resource);
    commandList->ResourceBarrier(1, &barrier);
}

///==========================================================
/// エイリアシングバリア
///==========================================================
void DxResourceBarrier::AliasingBarrier(
    ID3D12GraphicsCommandList* commandList,
    ID3D12Resource* beforeResource,
    ID3D12Resource* afterResource) {

    assert(commandList != nullptr);

    D3D12_RESOURCE_BARRIER barrier = CreateAliasingBarrier(beforeResource, afterResource);
    commandList->ResourceBarrier(1, &barrier);
}

///==========================================================
/// 現在の状態を取得
///==========================================================
D3D12_RESOURCE_STATES DxResourceBarrier::GetCurrentState(ID3D12Resource* resource) const {
    auto it = resourceStates_.find(resource);
    if (it != resourceStates_.end()) {
        return it->second;
    }
    return D3D12_RESOURCE_STATE_COMMON;
}

///==========================================================
/// 現在の状態を手動で更新
///==========================================================
void DxResourceBarrier::UpdateState(ID3D12Resource* resource, D3D12_RESOURCE_STATES newState) {
    assert(resource != nullptr);
    resourceStates_[resource] = newState;
}

///==========================================================
/// 状態遷移バリアを作成
///==========================================================
D3D12_RESOURCE_BARRIER DxResourceBarrier::CreateTransitionBarrier(
    ID3D12Resource* resource,
    D3D12_RESOURCE_STATES beforeState,
    D3D12_RESOURCE_STATES afterState,
    UINT subresource) {

    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource   = resource;
    barrier.Transition.StateBefore = beforeState;
    barrier.Transition.StateAfter  = afterState;
    barrier.Transition.Subresource = subresource;

    return barrier;
}

///==========================================================
/// UAVバリアを作成
///==========================================================
D3D12_RESOURCE_BARRIER DxResourceBarrier::CreateUAVBarrier(ID3D12Resource* resource) {
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type          = D3D12_RESOURCE_BARRIER_TYPE_UAV;
    barrier.Flags         = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.UAV.pResource = resource;

    return barrier;
}

///==========================================================
/// エイリアシングバリアを作成
///==========================================================
D3D12_RESOURCE_BARRIER DxResourceBarrier::CreateAliasingBarrier(
    ID3D12Resource* beforeResource,
    ID3D12Resource* afterResource) {

    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type                     = D3D12_RESOURCE_BARRIER_TYPE_ALIASING;
    barrier.Flags                    = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Aliasing.pResourceBefore = beforeResource;
    barrier.Aliasing.pResourceAfter  = afterResource;

    return barrier;
}
