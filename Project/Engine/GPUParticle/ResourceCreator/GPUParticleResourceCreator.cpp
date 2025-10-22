#include "GPUParticleResourceCreator.h"
#include "base/SrvManager.h"
#include "Dx/DirectXCommon.h"
#include "Frame/Frame.h"

void GPUParticleResourceCreator::Create() {
    dxCommon_   = DirectXCommon::GetInstance();
    srvManager_ = SrvManager::GetInstance();

    CreateParticleResource();
    CreateEmitterResource();
    CreatePerViewResource();
    CreatePerFrameResource();
    CreateCounterResource();
}

void GPUParticleResourceCreator::CreateParticleResource() {
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

void GPUParticleResourceCreator::CreateEmitterResource() {
    // Emitterデータ用のバッファ
    emitResource_ = dxCommon_->CreateBufferResource(
        dxCommon_->GetDevice(),
        sizeof(EmitterSphere) * particleMaxCount_);

    // マップしておく
    emitResource_->Map(0, nullptr, reinterpret_cast<void**>(&emitSphere_));
}

void GPUParticleResourceCreator::CreatePerViewResource() {
    // PerViewデータ用のバッファ
    perViewResource_ = dxCommon_->CreateBufferResource(
        dxCommon_->GetDevice(),
        sizeof(PerView) * particleMaxCount_);

    // マップしておく
    perViewResource_->Map(0, nullptr, reinterpret_cast<void**>(&perViewData_));
}

void GPUParticleResourceCreator::CreatePerFrameResource() {
    // PerFrameデータ用のバッファ
    perFrameResource_ = dxCommon_->CreateBufferResource(
        dxCommon_->GetDevice(), sizeof(PerFrame) * particleMaxCount_);

    perFrameResource_->Map(0, nullptr, reinterpret_cast<void**>(&perFrameData_));
}

void GPUParticleResourceCreator::CreateCounterResource() {
    // ParticleCS用のBufferを作成
    counterResource_ = dxCommon_->CreateBufferResource(
        dxCommon_->GetDevice(), sizeof(int32_t),
        ViewType::UnorderedAccess);

    // UAV作成
    uint32_t uavIndex         = srvManager_->Allocate();
    counterUavHandle_.first  = srvManager_->GetCPUDescriptorHandle(uavIndex);
    counterUavHandle_.second = srvManager_->GetGPUDescriptorHandle(uavIndex);

    srvManager_->CreateStructuredUAV(
        uavIndex, counterResource_.Get(),
        1, sizeof(int32_t));

 }

void GPUParticleResourceCreator::PerFrameIncrement() {
    perFrameData_->deltaTime = Frame::DeltaTime();
    perFrameData_->time += perFrameData_->deltaTime;
}