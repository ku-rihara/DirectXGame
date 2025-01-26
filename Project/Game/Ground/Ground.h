#pragma once
#include "3d/Object3d.h"
#include "3d/WorldTransform.h"
class Ground {
public:

private:
	// ワールド変換データ
	WorldTransform transform_;
	// モデル
	std::unique_ptr<Object3d> objct3D_=nullptr;

	
public:
	Ground();
	~Ground();
	void Init();
	void Update();
	void Draw(ViewProjection& viewProjection);

	void Debug();
};
