#include "GPUParticleResourceData.h"
#include "base/SrvManager.h"
#include "Dx/DirectXCommon.h"

void GPUParticleResourceData::Create() {
    dxCommon_   = DirectXCommon::GetInstance();
    srvManager_ = SrvManager::GetInstance();

    // commandBinder生成
    commandExecutor_ = std::make_unique<GPUParticleCommandExecutor>();
    commandExecutor_->Init(this);

    // Particleリソースの作成
    CreateParticleResource();
    CreateEmitterResource();
    CreatePerViewResource();
    CreatePerFrameResource();
    CreateFreeListIndexResource();
    CreateFreeListResource();
    CreateEmitParamResource();
}

void GPUParticleResourceData::CreateParticleResource() {
    // ParticleCS用のBufferを作成
    particleBuffer_.resource = dxCommon_->CreateBufferResource(
        dxCommon_->GetDevice(), sizeof(ParticleCS) * particleMaxCount_,
        ViewType::UnorderedAccess);

    // UAV作成
    uint32_t uavIndex      = srvManager_->Allocate();
    particleUAV_.cpuHandle = srvManager_->GetCPUDescriptorHandle(uavIndex);
    particleUAV_.gpuHandle = srvManager_->GetGPUDescriptorHandle(uavIndex);

    srvManager_->CreateStructuredUAV(
        uavIndex, particleBuffer_.resource.Get(),
        particleMaxCount_, sizeof(ParticleCS));

    // SRVを作成
    uint32_t srvIndex         = srvManager_->Allocate();
    particleBuffer_.cpuHandle = srvManager_->GetCPUDescriptorHandle(srvIndex);
    particleBuffer_.gpuHandle = srvManager_->GetGPUDescriptorHandle(srvIndex);

    srvManager_->CreateForStructuredBuffer(
        srvIndex,
        particleBuffer_.resource.Get(),
        particleMaxCount_,
        sizeof(ParticleCS));
}

void GPUParticleResourceData::CreateEmitterResource() {
    // Emitterデータ用のバッファ
    emitterBuffer_.resource = dxCommon_->CreateBufferResource(
        dxCommon_->GetDevice(),
        sizeof(ParticleEmit));

    // マップしておく
    emitterBuffer_.resource->Map(0, nullptr, reinterpret_cast<void**>(&emitterBuffer_.mappedData));
}

void GPUParticleResourceData::CreatePerViewResource() {
    // PerViewデータ用のバッファ
    perViewBuffer_.resource = dxCommon_->CreateBufferResource(
        dxCommon_->GetDevice(),
        sizeof(PerView));

    // マップしておく
    perViewBuffer_.resource->Map(0, nullptr, reinterpret_cast<void**>(&perViewBuffer_.mappedData));
}

void GPUParticleResourceData::CreatePerFrameResource() {
    // PerFrameデータ用のバッファ
    perFrameBuffer_.resource = dxCommon_->CreateBufferResource(
        dxCommon_->GetDevice(), sizeof(PerFrame));

    perFrameBuffer_.resource->Map(0, nullptr, reinterpret_cast<void**>(&perFrameBuffer_.mappedData));
}

void GPUParticleResourceData::CreateEmitParamResource() {
    // Emitterデータ用のバッファ
    emitParamBuffer_.resource = dxCommon_->CreateBufferResource(
        dxCommon_->GetDevice(),
        sizeof(EmitParameter));

    // マップしておく
    emitParamBuffer_.resource->Map(0, nullptr, reinterpret_cast<void**>(&emitParamBuffer_.mappedData));
}

void GPUParticleResourceData::CreateFreeListIndexResource() {
    // FreeListIndex用のBufferを作成
    freeListIndexBuffer_.resource = dxCommon_->CreateBufferResource(
        dxCommon_->GetDevice(), sizeof(int32_t),
        ViewType::UnorderedAccess);

    // UAV作成
    uint32_t uavIndex        = srvManager_->Allocate();
    freeListIndexBuffer_.cpuHandle = srvManager_->GetCPUDescriptorHandle(uavIndex);
    freeListIndexBuffer_.gpuHandle = srvManager_->GetGPUDescriptorHandle(uavIndex);

    srvManager_->CreateStructuredUAV(
        uavIndex, freeListIndexBuffer_.resource.Get(),
        1, sizeof(int32_t));
}

void GPUParticleResourceData::CreateFreeListResource() {
    // FreeList用のBufferを作成
    freeListBuffer_.resource = dxCommon_->CreateBufferResource(
        dxCommon_->GetDevice(), sizeof(int32_t) * particleMaxCount_,
        ViewType::UnorderedAccess);

    // UAV作成
    uint32_t uavIndex   = srvManager_->Allocate();
    freeListBuffer_.cpuHandle = srvManager_->GetCPUDescriptorHandle(uavIndex);
    freeListBuffer_.gpuHandle = srvManager_->GetGPUDescriptorHandle(uavIndex);

    srvManager_->CreateStructuredUAV(
        uavIndex, freeListBuffer_.resource.Get(),
        particleMaxCount_, sizeof(int32_t));
}


void GPUParticleResourceData::UpdateEmitterData(const ParticleEmit& data) {
    if (emitterBuffer_.IsValid()) {
        *emitterBuffer_.mappedData = data;
    }
}

void GPUParticleResourceData::UpdatePerViewData(const PerView& data) {
    if (perViewBuffer_.IsValid()) {
        *perViewBuffer_.mappedData = data;
    }
}

void GPUParticleResourceData::UpdatePerFrameData(float deltaTime) {
    if (perFrameBuffer_.IsValid()) {
        perFrameBuffer_.mappedData->deltaTime = deltaTime;
        perFrameBuffer_.mappedData->time += deltaTime;
    }
}


 GPUParticleCommandExecutor* GPUParticleResourceData::GetCommandExecutorRef() const {
    return commandExecutor_ ? commandExecutor_.get() : nullptr;
  }

 void GPUParticleResourceData::UpdateEmitParamData(const EmitParameter& data) {
      if (emitParamBuffer_.IsValid()) {
          *emitParamBuffer_.mappedData = data;
      }
  }