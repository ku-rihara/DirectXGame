#pragma once
#include "3d/Object3d.h"
#include "3d/WorldTransform.h"
#include"3d/ViewProjection.h"
class Field {
private:
	// ワールド変換データ
	WorldTransform transform_;
	// モデル
	std::unique_ptr<Object3d>obj3d_ = nullptr;

public:

	static Vector3 baseScale_;

	Field();
	~Field();
	/// ===================================================
	///public method
	/// ===================================================

	void Init();
	void Update();
	void Draw(ViewProjection& viewProjection);
};
