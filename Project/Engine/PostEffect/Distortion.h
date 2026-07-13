#pragma once

#include "BasePostEffect.h"
#include <d3d12.h>

namespace KetaEngine {

/// <summary>
/// 時空歪みポストエフェクト
/// </summary>
class Distortion : public BasePostEffect {
private:
    void CreateGraphicsPipeline() override;
    void CreateRootSignature() override;

public:
    Distortion()           = default;
    ~Distortion() override = default;

    // 初期化
    void Init(DirectXCommon* dxCommon) override;

    // 描画ステート設定、描画
    void SetDrawState(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;

    // 定数バッファ作成、ImGuiデバッグ表示
    void CreateConstantBuffer() override;
    void DebugParamImGui() override;

    // 歪みテクスチャのSRVハンドルを設定
    void SetDistortionSRV(const D3D12_GPU_DESCRIPTOR_HANDLE& handle) { distortionSrvHandle_ = handle; }

private:
    D3D12_GPU_DESCRIPTOR_HANDLE distortionSrvHandle_{};
};

} // namespace KetaEngine
