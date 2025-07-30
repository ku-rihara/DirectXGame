#pragma once
#include "3d/Object3d.h"
#include "3d/WorldTransform.h"
class Plane {
public:

private:
	// ワールド変換データ
	WorldTransform transform_;
	// モデル
	std::unique_ptr<Object3d> objct3D_=nullptr;

	// ワールド変換データ
	WorldTransform gtransform_;
	// モデル
	std::unique_ptr<Object3d> gobjct3D_ = nullptr;

	
public:
	Plane();
	~Plane();
	void Init();
	void Update();
	void Draw(ViewProjection& viewProjection);
    void Draws(ViewProjection& viewProjection);

	void Debug();
};
