#pragma once
#include "Model.h"
#include "WorldTransform.h"
class Skydome {
private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;

public:
	Skydome();
	~Skydome();
	void Init(Model* model);
	void Update();
	void Draw(ViewProjection& viewProjection);
};
