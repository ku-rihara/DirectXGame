#include "Field.h"
#include "Light/LightingType.h"

Vector3 Field::baseScale_ = {70, 1, 70};

void Field::Init() {

    gpuParticlePlayer_.Init();
    gpuParticlePlayer_.InitEffect("fire", "Field");
}

void Field::Update() {
    gpuParticlePlayer_.Play("fire", "Field");
    gpuParticlePlayer_.Update();
}
