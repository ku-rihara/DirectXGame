#include "GPUParticleCommandExecutor.h"
#include "base/SrvManager.h"
#include "Dx//DxResourceBarrier.h"
#include "Dx/DirectXCommon.h"
#include "GPUParticleResourceData.h"
#include "Pipeline/CSPipelineManager.h"

void GPUParticleCommandExecutor::Init(GPUParticleResourceData* resourceData) {
    resourceData_ = resourceData;

    dxCommon_      = DirectXCommon::GetInstance();
    csPipeManager_ = CSPipelineManager::GetInstance();
    srvManager_    = SrvManager::GetInstance();
}

void GPUParticleCommandExecutor::ExecuteInitPass(ID3D12GraphicsCommandList* commandList, const UINT& numThreadsX) {

    DxResourceBarrier* barrier = dxCommon_->GetResourceBarrier();

    //  デスクリプタヒープを設定
    ID3D12DescriptorHeap* descriptorHeaps[] = {srvManager_->GetDescriptorHeap()};
    commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

    // InitParticleパス
    csPipeManager_->PreDraw(CSPipelineType::Particle_Init, commandList);

    // u0: パーティクルバッファ(UAV)
    commandList->SetComputeRootDescriptorTable(0,
        resourceData_->GetParticleUAV().gpuHandle);

    // u1: FreeListIndex(UAV)
    commandList->SetComputeRootDescriptorTable(1,
        resourceData_->GetFreeListIndexBuffer().gpuHandle);

    // u2: FreeList(UAV)
    commandList->SetComputeRootDescriptorTable(2,
        resourceData_->GetFreeListBuffer().gpuHandle);

    csPipeManager_->DisPatch(CSPipelineType::Particle_Init, commandList, numThreadsX);

    // UAV barrier
    barrier->UAVBarrier(commandList, resourceData_->GetParticleBuffer().resource.Get());
}

void GPUParticleCommandExecutor::ExecuteEmitPass(ID3D12GraphicsCommandList* commandList, const UINT& numThreadsX) {
  
    DxResourceBarrier* barrier = dxCommon_->GetResourceBarrier();

    //  デスクリプタヒープを設定
    ID3D12DescriptorHeap* descriptorHeaps[] = {srvManager_->GetDescriptorHeap()};
    commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

    // エミットパス
    csPipeManager_->PreDraw(CSPipelineType::Particle_Emit, commandList);

    // b0: エミッターデータ
    commandList->SetComputeRootConstantBufferView(0,
        resourceData_->GetEmitterBuffer().resource->GetGPUVirtualAddress());

    // b1: PerFrame
    commandList->SetComputeRootConstantBufferView(2,
        resourceData_->GetPerFrameBuffer().resource->GetGPUVirtualAddress());

    // u0: パーティクルバッファ(UAV)
    commandList->SetComputeRootDescriptorTable(1,
        resourceData_->GetParticleUAV().gpuHandle);

    // u1: FreeListIndex(UAV)
    commandList->SetComputeRootDescriptorTable(3,
        resourceData_->GetFreeListIndexBuffer().gpuHandle);

    // u2: FreeList(UAV)
    commandList->SetComputeRootDescriptorTable(4,
        resourceData_->GetFreeListBuffer().gpuHandle);

    // 1スレッドでエミット処理
    csPipeManager_->DisPatch(CSPipelineType::Particle_Emit, commandList, numThreadsX);

    // UAV barrier
    barrier->UAVBarrier(commandList, resourceData_->GetParticleBuffer().resource.Get());
}

void GPUParticleCommandExecutor::ExecuteUpdatePass(ID3D12GraphicsCommandList* commandList, const UINT& numThreadsX) {
 
    DxResourceBarrier* barrier             = dxCommon_->GetResourceBarrier();

    // デスクリプタヒープを設定
    ID3D12DescriptorHeap* descriptorHeaps[] = {srvManager_->GetDescriptorHeap()};
    commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

    // 更新パス
    csPipeManager_->PreDraw(CSPipelineType::Particle_Update, commandList);

    // u0: パーティクルバッファ(UAV)
    commandList->SetComputeRootDescriptorTable(0,
        resourceData_->GetParticleUAV().gpuHandle);

    // b0: PerFrame
    commandList->SetComputeRootConstantBufferView(1,
        resourceData_->GetPerFrameBuffer().resource->GetGPUVirtualAddress());

    // u1: FreeListIndex(UAV)
    commandList->SetComputeRootDescriptorTable(2,
        resourceData_->GetFreeListIndexBuffer().gpuHandle);

    // u2: FreeList(UAV)
    commandList->SetComputeRootDescriptorTable(3,
        resourceData_->GetFreeListBuffer().gpuHandle);

    // 全パーティクルを更新
    csPipeManager_->DisPatch(CSPipelineType::Particle_Update, commandList, numThreadsX);

    // UAV barrier
    barrier->UAVBarrier(commandList, resourceData_->GetParticleBuffer().resource.Get());
}
