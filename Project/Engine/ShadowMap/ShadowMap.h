#pragma once

#include "ShadowMapData.h"
#include <d3d12.h>
#include <functional>
#include <memory>
#include <vector>
#include <wrl/client.h>
#include <Matrix4x4.h>

class SrvManager;
class DsvManager;
class ShadowMapPipeline;
class DirectXCommon;

class ShadowMap {
private:
    ShadowMap()                            = default;
    ~ShadowMap()                           = default;
    ShadowMap(const ShadowMap&)            = delete;
    ShadowMap& operator=(const ShadowMap&) = delete;

public:
    // シングルトンインスタンスの取得
    static ShadowMap* GetInstance();

    void Init(DirectXCommon* dxCommon);
    void CreateConstantBuffer();
    void CreateSRVHandle();
    void CreateDSVHandle();
    void CreateShadowMapResource(uint32_t width, uint32_t height);
    void UpdateLightMatrix();

    void PreDraw();
    void PostDraw();

    void Finalize();

    void DebugImGui();

private:
    DirectXCommon* dxCommon_;
    SrvManager* srvManager_;
    DsvManager* dsvManager_;
    std::unique_ptr<ShadowMapPipeline> pipeline_;

    // シャドウマップ関連
    Microsoft::WRL::ComPtr<ID3D12Resource> shadowMapResource_;
    D3D12_CPU_DESCRIPTOR_HANDLE shadowMapDsvHandle_;
    D3D12_GPU_DESCRIPTOR_HANDLE shadowMapSrvGPUHandle_;
    D3D12_CPU_DESCRIPTOR_HANDLE shadowMapSrvCPUHandle_;
    uint32_t shadowMapDsvIndex_;
    uint32_t shadowMapSrvIndex_;

    // リソース状態管理を追加
    D3D12_RESOURCE_STATES currentShadowMapState_;

    // シャドウマップのサイズ
    uint32_t shadowMapWidth_;
    uint32_t shadowMapHeight_;

    // ビューポートとシザー矩形
    D3D12_VIEWPORT shadowMapViewport_;
    D3D12_RECT shadowMapScissorRect_;

    // 定数バッファ
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
    ShadowTransformData* transformData_;
    float lightDistance_;

    // 深度バッファのSRV
    D3D12_GPU_DESCRIPTOR_HANDLE depthTextureGPUSrvHandle_;
    D3D12_CPU_DESCRIPTOR_HANDLE depthTextureCPUSrvHandle_;

    // ワールド行列用の定数バッファリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> worldMatrixResource_;
    WorldMatrixData* worldMatrixData_;

    void TransitionResourceState(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES newState);

public:
    // Getter methods
    ID3D12Resource* GetVertexResource() const { return vertexResource_.Get(); }
    ID3D12Resource* GetShadowMapResource() const { return shadowMapResource_.Get(); }
    D3D12_GPU_DESCRIPTOR_HANDLE GetShadowMapSrvGPUHandle() const { return shadowMapSrvGPUHandle_; }
    D3D12_CPU_DESCRIPTOR_HANDLE GetShadowMapDsvHandle() const { return shadowMapDsvHandle_; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetDepthTextureGPUHandle() const { return depthTextureGPUSrvHandle_; }
    uint32_t GetShadowMapWidth() const { return shadowMapWidth_; }
    uint32_t GetShadowMapHeight() const { return shadowMapHeight_; }
    D3D12_RESOURCE_STATES GetCurrentState() const { return currentShadowMapState_; }
    void SetWorldMatrix(Matrix4x4 m) { worldMatrixData_->world_ = m; }
    void SetLightCameraMatrix(Matrix4x4 m) { transformData_->lightCamera = m; }
};