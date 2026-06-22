#pragma once
#include"Matrix4x4.h"

namespace KetaEngine {

struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Matrix4x4 WorldInverseTranspose;
};

struct TransformationMatrix2D {
	Matrix4x4 WVP;
	Matrix4x4 World;
};

}; // KetaEngine
