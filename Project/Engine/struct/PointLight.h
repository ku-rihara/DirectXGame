#pragma once 
#include"Vector4.h"
#include"Vector3.h"
struct PointLight {
	Vector4 color;//ライトの色
	Vector3 position;//ライトの位置
	float intenesity;//輝度
	float radius;//ライトの幅
	float decay;//減衰率
	float padding[2];
};