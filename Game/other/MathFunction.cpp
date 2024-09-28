#include "MathFunction.h"
#include <cmath>

float LerpShortAngle(float a, float b, float t) {
	// 角度差分を求める
	float diff = b - a;
	float pi = 3.141592f;
	// 角度を[-2PI,+2PI]に補正する
	diff = std::fmodf(diff, 2.0f * pi);
	// 角度を[-PI,PI]に補正する
	if (diff > pi) {
		diff -= 2.0f * pi;
	}
	else if (diff < -pi) {
		diff += 2.0f * pi;
	}
	return a + diff * t;
}