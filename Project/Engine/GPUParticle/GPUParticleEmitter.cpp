#include "GPUParticleEmitter.h"
#include "Frame/Frame.h"

void GPUParticleEmitter::Init() {
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