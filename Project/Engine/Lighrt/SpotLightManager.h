#pragma once
#include "SpotLight.h"
#include <memory>
#include <vector>
#include <string>
#include <cstdint>

class SpotLightManager {
public:
    SpotLightManager()  = default;
    ~SpotLightManager() = default;

    ///=========================================================================================
    ///  public  method
    ///=========================================================================================

    void Init(ID3D12Device* device);
    void Update();
    void Add(ID3D12Device* device,const int32_t number);
    void Remove(int index);
    void UpdateStructuredBuffer();

    void DebugImGui();
    void SetLightCommand(ID3D12GraphicsCommandList* commandList);
   

private:
    std::vector<std::unique_ptr<SpotLight>> spotLights_;

    // Structured Buffer用のリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> structuredBufferResource_;
    SpotLightData* structuredBufferData_;
    uint32_t srvIndex_;

    std::string groupName_ = "SpotLight";

public:
    ///=========================================================================================
    ///  getter  method
    ///=========================================================================================

    SpotLight* GetSpotLight(int num) { return spotLights_[num].get(); }
    size_t GetLightCount() const { return spotLights_.size(); }
    uint32_t GetSrvIndex() const { return srvIndex_; }
    std::vector<SpotLight*> GetLights();
  
};