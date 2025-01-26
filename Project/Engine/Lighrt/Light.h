#pragma once
#include <memory>
#include<wrl.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include <vector>
//struct
#include"Vector3.h"

struct CameraForGPU {
    Vector3 worldPosition_;
};

// Forward Declarations
class DirectionalLight;
class PointLightManager;
class SpotLightManager;
class AreaLightManager;
class AmbientLight;

class Light {
private:
    // 各ライトマネージャークラスのインスタンスを持つ
    std::unique_ptr<DirectionalLight> directionalLight_;
    std::unique_ptr<PointLightManager> pointLightManager_;
    std::unique_ptr<SpotLightManager> spotLightManager_;
    std::unique_ptr<AreaLightManager>areaLightManager_;
    std::unique_ptr<AmbientLight>ambientLight_;
    //虚面反射
    Microsoft::WRL::ComPtr<ID3D12Resource> cameraForGPUResource_;
    //鏡面反射用データ
    CameraForGPU* cameraForGPUData_;

public:
    static Light* GetInstance();
    void Init(ID3D12Device* device);
    void DebugImGui();

    // Getter for CameraForGPU resource
    ID3D12Resource* GetCameraForGPUResource() const;
    // 各ライトのリソースをまとめて取得
    void SetLightCommands(ID3D12GraphicsCommandList* commandList);

    // 各ライトのリソースを取得する関数
    ID3D12Resource* GetDirectionalLightResource() const;
    ID3D12Resource* GetCameraForGPUResources() const;
 
    void SetWorldCameraPos(const Vector3& pos);

    // ライトマネージャーを取得するためのメソッド
    PointLightManager* GetPointLightManager() { return pointLightManager_.get(); }
    SpotLightManager* GetSpotLightManager() { return spotLightManager_.get(); }
};