#pragma once

#include <cstdint>
#include <d3d12.h>
#include <wrl/client.h>

namespace KetaEngine {

class DirectXCommon;

/// <summary>
/// 歪みパス専用レンダーターゲット
/// トレイルが書き込んだUVオフセットを保持する
/// </summary>
class DistortionRenderTarget {
public:
    DistortionRenderTarget()  = default;
    ~DistortionRenderTarget() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="dxCommon">DirectXCommon</param>
    /// <param name="width">幅</param>
    /// <param name="height">高さ</param>
    void Init(DirectXCommon* dxCommon, uint32_t width, uint32_t height);

    /// <summary>
    /// 歪みパス開始
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    /// <param name="dsvHandle">深度バッファのDSVハンドル</param>
    void BeginDistortionPass(ID3D12GraphicsCommandList* commandList,
                             const D3D12_CPU_DESCRIPTOR_HANDLE& dsvHandle);

    /// <summary>
    /// 歪みパス終了
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    void EndDistortionPass(ID3D12GraphicsCommandList* commandList);

    /// <summary>
    /// 終了処理
    /// </summary>
    void Finalize();

    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSrvHandle() const { return gpuSrvHandle_; }

private:
    void CreateResource(uint32_t width, uint32_t height);
    void CreateRTV();
    void CreateSRV();

private:
    DirectXCommon* dxCommon_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
    D3D12_GPU_DESCRIPTOR_HANDLE gpuSrvHandle_{};
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle_{};

    uint32_t rtvIndex_ = 0;

    // クリア値 (0,0,0,0) = 歪みなし
     const float kClearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
};

} // namespace KetaEngine
