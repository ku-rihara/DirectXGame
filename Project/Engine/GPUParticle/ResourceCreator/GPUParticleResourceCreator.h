#pragma once

#include "../Data/GPUParticleEmitterData.h"
#include "../Data/ParticleCSData.h"
#include <cstdint>
#include <d3d12.h>
#include <utility>
#include <wrl/client.h>

class DirectXCommon;
class SrvManager;

class GPUParticleResourceCreator {
public:
    GPUParticleResourceCreator()  = default;
    ~GPUParticleResourceCreator() = default;

    // リソース作成
    void Create();

    void PerFrameIncrement();

private:
    // Resource作成関数
    void CreateParticleResource();
    void CreateEmitterResource();
    void CreatePerViewResource();
    void CreatePerFrameResource();
    void CreateCounterResource();

private:
    DirectXCommon* dxCommon_;
    SrvManager* srvManager_;

    // Resource
    Microsoft::WRL::ComPtr<ID3D12Resource> particleResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> perViewResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> emitResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> perFrameResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> counterResource_;

    // Handle
    std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> particleSrvHandle_;
    std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> particleUavHandle_;

    std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> counterSrvHandle_;
    std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> counterUavHandle_;


    // Mapped data pointers
    EmitterSphere* emitSphere_ = nullptr;
    PerView* perViewData_      = nullptr;
    PerFrame* perFrameData_    = nullptr;
    int32_t* counterData_      = nullptr;

    int32_t particleMaxCount_;

public:
    // Getters
    ID3D12Resource* GetParticleResource() const { return particleResource_.Get(); }
    ID3D12Resource* GetEmitterResource() const { return emitResource_.Get(); }
    ID3D12Resource* GetPerViewResource() const { return perViewResource_.Get(); }
    ID3D12Resource* GetPerFrameResource() const { return perFrameResource_.Get(); }
    ID3D12Resource* GetCounterResource() const { return counterResource_.Get(); }

    D3D12_GPU_DESCRIPTOR_HANDLE GetParticleSrvHandle() const { return particleSrvHandle_.second; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetParticleUavHandle() const { return particleUavHandle_.second; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetCounterSrvHandle() const { return counterSrvHandle_.second; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetCounterUavHandle() const { return counterUavHandle_.second; }

    EmitterSphere* GetEmitterData() { return emitSphere_; }
    PerView* GetPerViewData() { return perViewData_; }
    PerFrame* GetPerFrameData() { return perFrameData_; }

    // setter
    void SetParticleCountMax(const int32_t& maxCount) { particleMaxCount_ = maxCount; }
};