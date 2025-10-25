#include "GPUParticleResourceData.h"
#include "base/SrvManager.h"
#include "Dx/DirectXCommon.h"
#include "Frame/Frame.h"

void GPUParticleResourceData::Create() {
    dxCommon_   = DirectXCommon::GetInstance();
    srvManager_ = SrvManager::GetInstance();

    CreateParticleResource();
    CreateEmitterResource();
    CreatePerViewResource();
    CreatePerFrameResource();
    CreateFreeListIndexResource();
    CreateFreeListResource();
}

void GPUParticleResourceData::CreateParticleResource() {
    // ParticleCS用のBufferを作成
    particleResource_ = dxCommon_->CreateBufferResource(
        dxCommon_->GetDevice(), sizeof(ParticleCS) * particleMaxCount_,
        ViewType::UnorderedAccess);

    // UAV作成
    uint32_t uavIndex         = srvManager_->Allocate();
    particleUavHandle_.first  = srvManager_->GetCPUDescriptorHandle(uavIndex);
    particleUavHandle_.second = srvManager_->GetGPUDescriptorHandle(uavIndex);

    srvManager_->CreateStructuredUAV(
        uavIndex, particleResource_.Get(),
        particleMaxCount_, sizeof(ParticleCS));

    // SRVを作成
    uint32_t srvIndex         = srvManager_->Allocate();
    particleSrvHandle_.first  = srvManager_->GetCPUDescriptorHandle(srvIndex);
    particleSrvHandle_.second = srvManager_->GetGPUDescriptorHandle(srvIndex);

    srvManager_->CreateForStructuredBuffer(
        srvIndex,
        particleResource_.Get(),
        particleMaxCount_,
        sizeof(ParticleCS));
}

void GPUParticleResourceData::CreateEmitterResource() {
    // Emitterデータ用のバッファ
    emitResource_ = dxCommon_->CreateBufferResource(
        dxCommon_->GetDevice(),
        sizeof(EmitterSphere) * particleMaxCount_);

    // マップしておく
    emitResource_->Map(0, nullptr, reinterpret_cast<void**>(&emitSphere_));
}

void GPUParticleResourceData::CreatePerViewResource() {
    // PerViewデータ用のバッファ
    perViewResource_ = dxCommon_->CreateBufferResource(
        dxCommon_->GetDevice(),
        sizeof(PerView) * particleMaxCount_);

    // マップしておく
    perViewResource_->Map(0, nullptr, reinterpret_cast<void**>(&perViewData_));
}

void GPUParticleResourceData::CreatePerFrameResource() {
    // PerFrameデータ用のバッファ
    perFrameResource_ = dxCommon_->CreateBufferResource(
        dxCommon_->GetDevice(), sizeof(PerFrame) * particleMaxCount_);

    perFrameResource_->Map(0, nullptr, reinterpret_cast<void**>(&perFrameData_));
}

void GPUParticleResourceData::CreateFreeListIndexResource() {
    // ParticleCS用のBufferを作成
    freeListIndexResource_ = dxCommon_->CreateBufferResource(
        dxCommon_->GetDevice(), sizeof(int32_t),
        ViewType::UnorderedAccess);

    // UAV作成
    uint32_t uavIndex              = srvManager_->Allocate();
    freeListIndexUavHandle_.first  = srvManager_->GetCPUDescriptorHandle(uavIndex);
    freeListIndexUavHandle_.second = srvManager_->GetGPUDescriptorHandle(uavIndex);

    srvManager_->CreateStructuredUAV(
        uavIndex, freeListIndexResource_.Get(),
        1, sizeof(int32_t));
}

void GPUParticleResourceData::CreateFreeListResource() {
    // ParticleCS用のBufferを作成
    freeListResource_ = dxCommon_->CreateBufferResource(
        dxCommon_->GetDevice(), sizeof(int32_t) * particleMaxCount_,
        ViewType::UnorderedAccess);

    // UAV作成
    uint32_t uavIndex         = srvManager_->Allocate();
    freeListSrvHandle_.first  = srvManager_->GetCPUDescriptorHandle(uavIndex);
    freeListUavHandle_.second = srvManager_->GetGPUDescriptorHandle(uavIndex);

    srvManager_->CreateStructuredUAV(
        uavIndex, freeListResource_.Get(),
        particleMaxCount_, sizeof(int32_t));
}

void GPUParticleResourceData::PerFrameIncrement() {
    perFrameData_->deltaTime = Frame::DeltaTime();
    perFrameData_->time += perFrameData_->deltaTime;
}