#pragma once
#include<cstdint>
#include"Vector4.h"

struct Material {
	Vector4 color;
	int32_t enableLighting;
};