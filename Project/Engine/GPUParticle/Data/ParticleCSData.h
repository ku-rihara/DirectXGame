#pragma once
#include"Vector3.h"
#include"Vector4.h"
#include"Matrix4x4.h"

struct ParticleCS {
    Vector3 translate;
    Vector3 scale;
    float lifeTime;
    Vector3 velocity;
    float currentTime;
    Vector4 color;
};

struct PerView {
    Matrix4x4 viewProjection;
    Matrix4x4 billboardMatrix;
};