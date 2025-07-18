#pragma once
#include "PointLight.h"
#include <memory>
#include <vector>

class PointLightManager {
private:
    std::vector<std::unique_ptr<PointLight>> pointLights_;

    // Structured Buffer用のリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> structuredBufferResource_;
    PointLightData* structuredBufferData_;
    uint32_t srvIndex_;

public:
    PointLightManager()  = default;
    ~PointLightManager() = default;

    ///=========================================================================================
    ///  public  method
    ///=========================================================================================

    void Init(ID3D12Device* device); // 初期化メソッドを追加
    void Add(ID3D12Device* device);
    void Remove(int index);
    void UpdateStructuredBuffer(); // Structured Bufferの更新

    void DebugImGui();
    std::vector<PointLight*> GetLights();
    void SetLightCommand(ID3D12GraphicsCommandList* commandList);

    ///=========================================================================================
    ///  getter  method
    ///=========================================================================================

    PointLight* GetPointLight(int num) { return pointLights_[num].get(); }
    size_t GetLightCount() const { return pointLights_.size(); }
   
};