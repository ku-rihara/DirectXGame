#pragma once

#include"Vector3.h"
#include<cstdint>

struct EmitterSphere {
    Vector3 translate;
    float radius;
    uint32_t count;
    float frequency;
    float frequencyTime;
    uint32_t emit;
};

struct PerFrame {
    float time;
    float deltaTime;
};