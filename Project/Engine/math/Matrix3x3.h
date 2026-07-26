#pragma once

/// <summary>
/// 3x3行列構造体
/// </summary>
struct Matrix3x3 {
	float m[3][3];
};

Matrix3x3 MakeIdentity3x3();
