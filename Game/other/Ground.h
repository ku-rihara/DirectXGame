#pragma once
#include "Model.h"
#include "WorldTransform.h"
class Ground {
private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;

public:
	Ground();
	~Ground();
	void Init(Model* model);
	void Update();
	void Draw(ViewProjection& viewProjection);
};
