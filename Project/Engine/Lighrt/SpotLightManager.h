#pragma once
#include "SpotLight.h"
#include <memory>
#include <vector>

class SpotLightManager {
private:
    std::vector<std::unique_ptr<SpotLight>> spotLights_;

    // Structured Buffer用のリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> structuredBufferResource_;
    SpotLightData* structuredBufferData_;
    uint32_t srvIndex_;

public:
    SpotLightManager()  = default;
    ~SpotLightManager() = default;

    ///=========================================================================================
    ///  public  method
    ///=========================================================================================

    void Init(ID3D12Device* device); // 初期化メソッドを追加
    void Add(ID3D12Device* device);
    void Remove(int index);
    void UpdateStructuredBuffer();

    void DebugImGui();
    std::vector<SpotLight*> GetLights();
    void SetLightCommand(ID3D12GraphicsCommandList* commandList);

    ///=========================================================================================
    ///  getter  method
    ///=========================================================================================

    SpotLight* GetSpotLight(int num) { return spotLights_[num].get(); }
    size_t GetLightCount() const { return spotLights_.size(); }
    uint32_t GetSrvIndex() const { return srvIndex_; }
};