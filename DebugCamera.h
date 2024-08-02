#pragma once
#include"Vector3.h"
#include"Matrix4x4.h"
class DebugCamera{
	//ローカル回転角
	Vector3 rotate = {};
	//ローカル座標
	Vector3 translation_ = { 0,0,-50 };

	//ビュー行列
	Matrix4x4 matView_ = {};
	//射影行列
	Matrix4x4 matProjection_ = {};
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();
};

