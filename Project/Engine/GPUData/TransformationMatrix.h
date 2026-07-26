#pragma once
#include"Matrix4x4.h"

namespace KetaEngine {

/// <summary>
/// 3Dオブジェクト用のWVP・World・法線変換行列を保持する定数バッファ用構造体
/// </summary>
struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Matrix4x4 WorldInverseTranspose;
};

/// <summary>
/// 2D描画用のWVP・World行列を保持する定数バッファ用構造体
/// </summary>
struct TransformationMatrix2D {
	Matrix4x4 WVP;
	Matrix4x4 World;
};

}; // KetaEngine
