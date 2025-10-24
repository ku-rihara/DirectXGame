#pragma once

#include "../Data/GPUParticleEmitterData.h"
#include "../Data/ParticleCSData.h"
#include <cstdint>
#include <d3d12.h>
#include <utility>
#include <wrl/client.h>

class DirectXCommon;
class SrvManager;

/// <summary>
/// GPUパーティクル用リソース作成クラス
/// </summary>
class GPUParticleResourceCreator {
public:
    GPUParticleResourceCreator()  = default;
    ~GPUParticleResourceCreator() = default;

    void Create(); //< 全リソースを作成
    void PerFrameIncrement(); //< フレームごとのデータ更新

private:
    void CreateParticleResource(); //< パーティクルバッファ作成
    void CreateEmitterResource(); //< エミッターバッファ作成
    void CreatePerViewResource(); //< ビュー情報バッファ作成
    void CreatePerFrameResource(); //< フレーム情報バッファ作成
    void CreateFreeListIndexResource(); //< フリーリストインデックスバッファ作成
    void CreateFreeListResource(); //< フリーリストバッファ作成

private:
    DirectXCommon* dxCommon_;
    SrvManager* srvManager_;

    Microsoft::WRL::ComPtr<ID3D12Resource> particleResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> perViewResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> emitResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> perFrameResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> freeListIndexResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> freeListResource_;

    std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> particleSrvHandle_;
    std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> particleUavHandle_;

    std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> freeListIndexSrvHandle_;
    std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> freeListIndexUavHandle_;

    std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> freeListSrvHandle_;
    std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> freeListUavHandle_;


    EmitterSphere* emitSphere_ = nullptr;
    PerView* perViewData_      = nullptr;
    PerFrame* perFrameData_    = nullptr;
    int32_t* counterData_      = nullptr;

    int32_t particleMaxCount_;

public:
    ID3D12Resource* GetParticleResource() const { return particleResource_.Get(); }
    ID3D12Resource* GetEmitterResource() const { return emitResource_.Get(); }
    ID3D12Resource* GetPerViewResource() const { return perViewResource_.Get(); }
    ID3D12Resource* GetPerFrameResource() const { return perFrameResource_.Get(); }
    ID3D12Resource* GetFreeListResource() const { return freeListIndexResource_.Get(); }

    D3D12_GPU_DESCRIPTOR_HANDLE GetParticleSrvHandle() const { return particleSrvHandle_.second; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetParticleUavHandle() const { return particleUavHandle_.second; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetFreeListIndexSrvHandle() const { return freeListIndexSrvHandle_.second; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetFreeListIndexUavHandle() const { return freeListIndexUavHandle_.second; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetFreeListSrvHandle() const { return freeListSrvHandle_.second; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetFreeListUavHandle() const { return freeListUavHandle_.second; }


    EmitterSphere* GetEmitterData() { return emitSphere_; }
    PerView* GetPerViewData() { return perViewData_; }
    PerFrame* GetPerFrameData() { return perFrameData_; }

    void SetParticleCountMax(const int32_t& maxCount) { particleMaxCount_ = maxCount; }
};