#include "GPUParticleResourceCreator.h"
#include "base/SrvManager.h"
#include "Dx/DirectXCommon.h"

void GPUParticleResourceCreator::Create() {
    dxCommon_   = DirectXCommon::GetInstance();
    srvManager_ = SrvManager::GetInstance();

    CreateParticleResource();
    CreateEmitterResource();
    CreatePerViewResource();
}

void GPUParticleResourceCreator::CreateParticleResource() {
    // ParticleCS用のBufferを作成
    particleResource_ = dxCommon_->CreateBufferResource(
        dxCommon_->GetDevice(),
        sizeof(ParticleCS) * particleMaxCount_,
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

void GPUParticleResourceCreator::CreateEmitterResource() {
    // Emitterデータ用のバッファ
    emitResource_ = dxCommon_->CreateBufferResource(
        dxCommon_->GetDevice(),
        sizeof(EmitterSphere));

    // マップしておく
    emitResource_->Map(0, nullptr, reinterpret_cast<void**>(&emitSphere_));
}

void GPUParticleResourceCreator::CreatePerViewResource() {
    // PerViewデータ用のバッファ
    perViewResource_ = dxCommon_->CreateBufferResource(
        dxCommon_->GetDevice(),
        sizeof(PerView));

    // マップしておく 
    perViewResource_->Map(0, nullptr, reinterpret_cast<void**>(&perViewData_));
}