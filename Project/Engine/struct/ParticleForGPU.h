#pragma once
#include"Matrix4x4.h"
#include"Vector4.h"
#include"3d/WorldTransform.h"

struct ParticleFprGPU {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Matrix4x4 WorldInverseTranspose;
	Vector4 color;
};

struct Particle {
	float lifeTime_;
	float currentTime_;
	Vector3 velocity_;
	Vector4 color_;
	WorldTransform worldTransform_;

};