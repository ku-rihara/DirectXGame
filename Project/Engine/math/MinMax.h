#pragma once

#include"Vector2.h"
#include"Vector3.h"
#include"Vector4.h"

/// <summary>
/// float値の最小・最大範囲
/// </summary>
struct FMinMax {
	float min;
	float max;
};

/// <summary>
/// Vector2の最小・最大範囲
/// </summary>
struct V2MinMax {
	Vector2 min;
	Vector2 max;
};

/// <summary>
/// Vector3の最小・最大範囲
/// </summary>
struct V3MinMax {
	Vector3 min;
	Vector3 max;
};

/// <summary>
/// Vector4の最小・最大範囲
/// </summary>
struct V4MinMax {
	Vector4 min;
	Vector4 max;
};