#pragma once
#include"Vector2.h"
#include"Vector4.h"
#include"Vector3.h"


namespace KetaEngine {

struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
	Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f); 
};



}; // KetaEngine
