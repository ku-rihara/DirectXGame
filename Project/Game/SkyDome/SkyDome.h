#pragma once
#include "3d/Object3d.h"
#include "3d/WorldTransform.h"

class Skydome {
private:
	// ワールド変換データ
	WorldTransform transform_;
	// モデル
	std::unique_ptr<Object3d> obj3D_ = nullptr;

public:
	Skydome();
	~Skydome();
	void Init();
	void Update();
	void Draw(ViewProjection& viewProjection);
};
