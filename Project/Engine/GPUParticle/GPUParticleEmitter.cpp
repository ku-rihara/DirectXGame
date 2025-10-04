#include "GPUParticleEmitter.h"
#include "Frame/Frame.h"

void GPUParticleEmitter::Init() {
    emitterSphere_.translate     = Vector3(0.0f, 0.0f, 0.0f);
    emitterSphere_.radius        = 1.0f;
    emitterSphere_.count         = 10;
    emitterSphere_.frequency     = 1.0f;
    emitterSphere_.frequencyTime = 0.0f;
    emitterSphere_.emit          = 0;
}

void GPUParticleEmitter::Update() {
    emitterSphere_.frequencyTime += Frame::DeltaTime();

    if (emitterSphere_.frequency <= emitterSphere_.frequencyTime) {
        emitterSphere_.frequencyTime -= emitterSphere_.frequency;
        emitterSphere_.emit = 1;
    } else {
        emitterSphere_.emit = 0;
    }
}

void GPUParticleEmitter::SetEmitterData(const EmitterSphere& data) {
    emitterSphere_ = data;
}

const EmitterSphere& GPUParticleEmitter::GetEmitterData() const {
    return emitterSphere_;
}

float GPUParticleEmitter::GetFrequencyTime() const {
    return emitterSphere_.frequencyTime;
}

uint32_t GPUParticleEmitter::GetEmitFlag() const {
    return emitterSphere_.emit;
}