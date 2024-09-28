#pragma once
#include"WorldTransform.h"
#include "Model.h"
class Effect {
private:
	// モデル配列データ
	Model* model_;
	// ワールド変換データ
	WorldTransform worldTransform_;
	float easeT_;
	bool isFinished_=false;
	
public:
	void Init(Model* model,const Vector3& Pos);
	void Update();
	void Draw(const ViewProjection&viewProjection);

	bool IsFinished() const;

};
