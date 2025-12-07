#pragma once
#include"Vector3.h"
#include"Vector4.h"
#include"Matrix4x4.h"

struct ParticleCS {
    Vector3 translate; 
    float lifeTime; 
    Vector3 scale; 
    float currentTime; 
    Vector3 rotate;
    Vector3 rotateSpeed; 
    Vector3 velocity; 
    Vector4 color;

};

struct PerView {
    Matrix4x4 viewProjection;
    Matrix4x4 billboardMatrix;
};