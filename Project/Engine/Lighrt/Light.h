#pragma once
#include <d3d12.h>
#include <memory>
#include <wrl.h>
// struct
#include "Vector3.h"

struct CameraForGPU {
    Vector3 worldPosition_;
};

struct LightCountData {
    int pointLightCount;
    int spotLightCount;
};

// 前方宣言
class DirectionalLight;
class PointLightManager;
class SpotLightManager;
class AreaLightManager;
class AmbientLight;
class DirectXCommon;

class Light {
public:
    static Light* GetInstance();
    ~Light() = default;

    void Init(DirectXCommon* dxCommon);
    void InitAllLights();
    void DebugImGui();

    //
    void AddSpotLight();
    void AddPointLight();

    void RemoveSpotLight(const int& num);
    void RemovePointLight(const int& num);

private:
    DirectXCommon* dxCommon_;
    // Lights
    std::unique_ptr<DirectionalLight> directionalLight_;
    std::unique_ptr<PointLightManager> pointLightManager_;
    std::unique_ptr<SpotLightManager> spotLightManager_;
    std::unique_ptr<AreaLightManager> areaLightManager_;
    std::unique_ptr<AmbientLight> ambientLight_;

    // 鏡面反射
    Microsoft::WRL::ComPtr<ID3D12Resource> cameraForGPUResource_;
    CameraForGPU* cameraForGPUData_;

    // LightsData
    Microsoft::WRL::ComPtr<ID3D12Resource> lightCountResource_;
    LightCountData* lightCountData_;

public:
    // getter
    PointLightManager* GetPointLightManager() { return pointLightManager_.get(); }
    SpotLightManager* GetSpotLightManager() { return spotLightManager_.get(); }
    AmbientLight* GetAmbientLight() { return ambientLight_.get(); }

    // setter
    void SetLightCommands(ID3D12GraphicsCommandList* commandList);
    void SetWorldCameraPos(const Vector3& pos);
};