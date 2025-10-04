#pragma once

#include "../Data/GPUParticleEmitterData.h"
#include"../Data/ParticleCSData.h"
#include <cstdint>
#include <utility>
#include <wrl/client.h>
#include <d3d12.h>

class DirectXCommon;
class SrvManager;
class GPUParticleResourceCreator {
public:
    GPUParticleResourceCreator()  = default;
    ~GPUParticleResourceCreator() = default;

    void Create();

private:
    void CreateParticleResource();
    void CreateEmitterResource();
    void CreatePerViewResource();

private:
    DirectXCommon* dxCommon_;
    SrvManager* srvManager_;

    //Resource
    Microsoft::WRL::ComPtr<ID3D12Resource> particleResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> perViewResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> emitResource_;

    // Handle
    std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> particleSrvHandle_;
    std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> particleUavHandle_;

    // data
    EmitterSphere emitSphere_;
    PerView perViewData_;

    int32_t particleMaxCount_;

public:
    void SetParticleCountMax(const int32_t& maxCount) { particleMaxCount_ = maxCount; } 
};