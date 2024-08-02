#pragma once
#include"Vector3.h"
/// <summary>
/// ワールド変換データ
/// </summary>
class WorldTransform{
public:
	//ローカルスケール
	Vector3 scale_ = { 1,1,1 };
	//ローカル回転角
	Vector3 rotation_ = {};
	//ローカル座標
	Vector3 translation_ = {};
	//ローカル→
};

