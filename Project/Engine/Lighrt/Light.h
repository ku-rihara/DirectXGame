#pragma once
#include <d3d12.h>
#include <memory>
#include <wrl.h>
// struct
#include "Vector3.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include <string>
#include <cstdint>

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
    void Update();
    void InitAllLights();
    void DebugImGui();

    //
    void AddSpotLight();
    void AddPointLight();

    void RemoveSpotLight(const int& num);
    void RemovePointLight(const int& num);

    void BindParams();

private:
    DirectXCommon* dxCommon_;
    GlobalParameter* globalParameter_; 
    const std::string groupName_ = "LightCount";

    int32_t spotLightCoutMax_;
    int32_t current;

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
    DirectionalLight* GetDirectionalLight() { return directionalLight_.get(); }
    Vector3 GetWorldCameraPos() const { return cameraForGPUData_->worldPosition_; }

    // setter
    void SetLightCommands(ID3D12GraphicsCommandList* commandList);
    void SetWorldCameraPos(const Vector3& pos);
};