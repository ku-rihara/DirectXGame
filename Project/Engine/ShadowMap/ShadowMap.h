#pragma once

#include "ShadowMapData.h"
#include "Vector3.h"
#include "Vector4.h"
#include <d3d12.h>
#include <functional>
#include <Matrix4x4.h>
#include <memory>
#include <vector>
#include <wrl/client.h>

class SrvManager;
class DsvManager;
class ShadowMapPipeline;
class DirectXCommon;
class ViewProjection;

/// <summary>
/// シャドウマップ管理クラス
/// </summary>
class ShadowMap {
private:
    ShadowMap()                            = default;
    ~ShadowMap()                           = default;
    ShadowMap(const ShadowMap&)            = delete;
    ShadowMap& operator=(const ShadowMap&) = delete;

public:
    static ShadowMap* GetInstance();

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="dxCommon">DirectXCommon</param>
    void Init(DirectXCommon* dxCommon);

    /// <summary>
    /// シャドウマップリソース作成
    /// </summary>
    /// <param name="width">幅</param>
    /// <param name="height">高さ</param>
    void CreateShadowMapResource(uint32_t width, uint32_t height);

    void CreateConstantBuffer(); //< 定数バッファ作成
    void CreateSRVHandle(); //< SRVハンドル作成
    void CreateDSVHandle(); //< DSVハンドル作成
    void UpdateLightMatrix(); //< ライト行列更新

    void PreDraw(); //< 描画前処理
    void PostDraw(); //< 描画後処理

    void Finalize(); //< 終了処理
    void DebugImGui(); //< ImGuiデバッグ

private:
    Vector3 GetLightDirectionAndPosition(); //< ライト方向と位置取得

    /// <summary>
    /// ライトの向き設定
    /// </summary>
    /// <param name="lightDirection">ライト方向</param>
    void SetLightOrientation(const Vector3& lightDirection);

    /// <summary>
    /// リソース状態遷移
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    /// <param name="newState">新しい状態</param>
    void TransitionResourceState(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES newState);

private:
    DirectXCommon* dxCommon_;
    SrvManager* srvManager_;
    DsvManager* dsvManager_;
    std::unique_ptr<ShadowMapPipeline> pipeline_;

    Microsoft::WRL::ComPtr<ID3D12Resource> shadowMapResource_;
    D3D12_CPU_DESCRIPTOR_HANDLE shadowMapDsvHandle_;
    D3D12_GPU_DESCRIPTOR_HANDLE shadowMapSrvGPUHandle_;
    D3D12_CPU_DESCRIPTOR_HANDLE shadowMapSrvCPUHandle_;
    uint32_t shadowMapDsvIndex_;
    uint32_t shadowMapSrvIndex_;

    // リソース状態管理
    D3D12_RESOURCE_STATES currentShadowMapState_;

    uint32_t shadowMapWidth_;
    uint32_t shadowMapHeight_;

    D3D12_VIEWPORT shadowMapViewport_;
    D3D12_RECT shadowMapScissorRect_;

    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
    ShadowTransformData* transformData_;
    float lightDistance_;

    D3D12_GPU_DESCRIPTOR_HANDLE depthTextureGPUSrvHandle_;
    D3D12_CPU_DESCRIPTOR_HANDLE depthTextureCPUSrvHandle_;

    Vector4 lightDirection_;
    Vector3 cameraPosition_;

    std::unique_ptr<ViewProjection> lightViewProjection_;
    Vector3 targetPos_;

public:
    ID3D12Resource* GetVertexResource() const { return vertexResource_.Get(); }
    ID3D12Resource* GetShadowMapResource() const { return shadowMapResource_.Get(); }
    D3D12_GPU_DESCRIPTOR_HANDLE GetShadowMapSrvGPUHandle() const { return shadowMapSrvGPUHandle_; }
    D3D12_CPU_DESCRIPTOR_HANDLE GetShadowMapDsvHandle() const { return shadowMapDsvHandle_; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetDepthTextureGPUHandle() const { return depthTextureGPUSrvHandle_; }
    uint32_t GetShadowMapWidth() const { return shadowMapWidth_; }
    uint32_t GetShadowMapHeight() const { return shadowMapHeight_; }
    D3D12_RESOURCE_STATES GetCurrentState() const { return currentShadowMapState_; }
    void SetLightCameraMatrix(Matrix4x4 m) { transformData_->lightCamera = m; }
};