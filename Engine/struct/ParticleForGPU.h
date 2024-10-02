#pragma once
#include"Matrix4x4.h"
#include"Vector4.h"

struct ParticleFprGPU {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Matrix4x4 WorldInverseTranspose;
	Vector4 color;
};