#pragma once
#include <d3d12.h>
#include <unordered_map>
#include <vector>
#include <wrl/client.h>

/// <summary>
/// リソースバリアを管理するクラス
/// </summary>
namespace KetaEngine {

class DxResourceBarrier {
public:
    DxResourceBarrier()  = default;
    ~DxResourceBarrier() = default;

    /// <summary>
    /// リソースの初期状態を登録
    /// </summary>
    /// <param name="resource">リソース</param>
    /// <param name="initialState">初期状態</param>
    void RegisterResource(ID3D12Resource* resource, D3D12_RESOURCE_STATES initialState);

    /// <summary>
    /// リソースの登録を解除
    /// </summary>
    /// <param name="resource">リソース</param>
    void UnregisterResource(ID3D12Resource* resource);

    /// <summary>
    /// 状態遷移バリアを設定
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    /// <param name="resource">リソース</param>
    /// <param name="afterState">遷移後の状態</param>
    /// <param name="subresource">サブリソース</param>
    void Transition(
        ID3D12GraphicsCommandList* commandList,
        ID3D12Resource* resource,
        D3D12_RESOURCE_STATES afterState,
        UINT subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

    /// <summary>
    /// 強制的に状態遷移バリアを設定
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    /// <param name="resource">リソース</param>
    /// <param name="beforeState">遷移前の状態</param>
    /// <param name="afterState">遷移後の状態</param>
    /// <param name="subresource">サブリソース</param>
    void TransitionForce(
        ID3D12GraphicsCommandList* commandList,
        ID3D12Resource* resource,
        D3D12_RESOURCE_STATES beforeState,
        D3D12_RESOURCE_STATES afterState,
        UINT subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

    /// <summary>
    /// UAVバリアを設定
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    /// <param name="resource">リソース</param>
    void UAVBarrier(
        ID3D12GraphicsCommandList* commandList,
        ID3D12Resource* resource);

    /// <summary>
    /// エイリアシングバリアを設定
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    /// <param name="beforeResource">前のリソース</param>
    /// <param name="afterResource">後のリソース</param>
    void AliasingBarrier(
        ID3D12GraphicsCommandList* commandList,
        ID3D12Resource* beforeResource,
        ID3D12Resource* afterResource);

    /// <summary>
    /// 現在の状態を取得
    /// </summary>
    /// <param name="resource">リソース</param>
    /// <returns>現在の状態</returns>
    D3D12_RESOURCE_STATES GetCurrentState(ID3D12Resource* resource) const;

    /// <summary>
    /// 現在の状態を更新
    /// </summary>
    /// <param name="resource">リソース</param>
    /// <param name="newState">新しい状態</param>
    void UpdateState(ID3D12Resource* resource, D3D12_RESOURCE_STATES newState);

private:

    /// <summary>
    /// 状態遷移バリアを作成
    /// </summary>
    /// <param name="resource">リソース</param>
    /// <param name="beforeState">遷移前の状態</param>
    /// <param name="afterState">遷移後の状態</param>
    /// <param name="subResource">サブリソース</param>
    /// <returns>リソースバリア</returns>
    D3D12_RESOURCE_BARRIER CreateTransitionBarrier(
        ID3D12Resource* resource,
        D3D12_RESOURCE_STATES beforeState,
        D3D12_RESOURCE_STATES afterState,
        UINT subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

    /// <summary>
    /// UAVバリアを作成
    /// </summary>
    /// <param name="resource">リソース</param>
    /// <returns>リソースバリア</returns>
    D3D12_RESOURCE_BARRIER CreateUAVBarrier(ID3D12Resource* resource);

    /// <summary>
    /// エイリアシングバリアを作成
    /// </summary>
    /// <param name="beforeResource">前のリソース</param>
    /// <param name="afterResource">後のリソース</param>
    /// <returns>リソースバリア</returns>
    D3D12_RESOURCE_BARRIER CreateAliasingBarrier(
        ID3D12Resource* beforeResource,
        ID3D12Resource* afterResource);

private:
    std::unordered_map<ID3D12Resource*, D3D12_RESOURCE_STATES> resourceStates_;
};

}; // KetaEngine
