#pragma once
#include "../Data/GPUParticleEmitterData.h"
#include "../Data/ParticleCSData.h"
#include "GPUParticleCommandExecutor.h"
#include <cstdint>
#include <d3d12.h>
#include <memory>
#include <utility>
#include <wrl/client.h>

namespace KetaEngine {

class DirectXCommon;
class SrvManager;

/// <summary>
/// GPUパーティクル用リソース管理クラス
/// </summary>
class GPUParticleResourceData {
public:
    struct GPUResourceHandle {
        Microsoft::WRL::ComPtr<ID3D12Resource> resource;
        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
        D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
        bool IsValid() const { return resource != nullptr; }
    };

    template <typename T>
    struct MappedResourceHandle {
        Microsoft::WRL::ComPtr<ID3D12Resource> resource;
        T* mappedData = nullptr;

        T* GetMappedData() { return mappedData; }
        const T* GetMappedData() const { return mappedData; }
        bool IsValid() const { return resource != nullptr && mappedData != nullptr; }
    };

    // Emit関連のバッファをまとめた構造体
    struct EmitParamBuffers {
        MappedResourceHandle<EmitTransformParams> transformBuffer;
        MappedResourceHandle<EmitPhysicsParams> physicsBuffer;
        MappedResourceHandle<EmitAppearanceParams> appearanceBuffer;
    };

public:
    GPUParticleResourceData()  = default;
    ~GPUParticleResourceData() = default;

    void Create(); //< リソースを作成

    /// <summary>
    /// エミッターデータを更新
    /// </summary>
    void UpdateEmitterData(const ParticleEmit& data);

    /// <summary>
    /// ビューデータを更新
    /// </summary>
    void UpdatePerViewData(const PerView& data);

    /// <summary>
    /// フレームデータを更新
    /// </summary>
    void UpdatePerFrameData(float deltaTime);

    /// <summary>
    /// Emit関連パラメータを一括更新
    /// </summary>
    void UpdateEmitParams(const EmitParamBuffers& params);

private:
    //*----------------------ヘルパー関数----------------------*//
    void CreateParticleResource();        //< パーティクルバッファ作成
    void CreateEmitterResource();         //< エミッターバッファ作成
    void CreatePerViewResource();         //< ビュー情報バッファ作成
    void CreatePerFrameResource();        //< フレーム情報バッファ作成
    void CreateFreeListIndexResource();   //< フリーリストインデックスバッファ作成
    void CreateFreeListResource();        //< フリーリストバッファ作成
    void CreateEmitParamBuffers();        //< Emit関連バッファ作成

    /// <summary>
    /// 定数バッファリソース作成のヘルパー
    /// </summary>
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

private:
    std::unique_ptr<GPUParticleCommandExecutor> commandExecutor_;

    DirectXCommon* dxCommon_ = nullptr;
    SrvManager* srvManager_  = nullptr;

    GPUResourceHandle particleBuffer_;
    GPUResourceHandle particleUAV_;
    GPUResourceHandle freeListIndexBuffer_;
    GPUResourceHandle freeListBuffer_;

    // Emit関連バッファ
    EmitParamBuffers emitParamBuffers_;

    MappedResourceHandle<ParticleEmit> emitterBuffer_;
    MappedResourceHandle<PerView> perViewBuffer_;
    MappedResourceHandle<PerFrame> perFrameBuffer_;

    int32_t particleMaxCount_ = 1024;

public:
    // getter
    const MappedResourceHandle<ParticleEmit>& GetEmitterBuffer() const { return emitterBuffer_; }
    const MappedResourceHandle<PerView>& GetPerViewBuffer() const { return perViewBuffer_; }
    const MappedResourceHandle<PerFrame>& GetPerFrameBuffer() const { return perFrameBuffer_; }

    // Emit関連バッファのgetter
    const EmitParamBuffers& GetEmitParamBuffers() const { return emitParamBuffers_; }
  
    GPUParticleCommandExecutor* GetCommandExecutorRef() const;

    const GPUResourceHandle& GetParticleBuffer() const { return particleBuffer_; }
    const GPUResourceHandle& GetParticleUAV() const { return particleUAV_; }
    const GPUResourceHandle& GetFreeListIndexBuffer() const { return freeListIndexBuffer_; }
    const GPUResourceHandle& GetFreeListBuffer() const { return freeListBuffer_; }

    // setter
    void SetParticleMaxCount(int32_t maxCount) { particleMaxCount_ = maxCount; }
};

}; // KetaEngine
