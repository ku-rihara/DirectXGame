#pragma once
#include"3d/WorldTransform.h"
#include "3d/Object3d.h"
class ImpactEffect {
private:
	// モデル配列データ
	std::unique_ptr<Object3d> obj3d_;
	// ワールド変換データ
	WorldTransform worldTransform_;
	float easeT_;
	float alpha_;
	bool isFinished_=false;
	
public:

	 ImpactEffect();
     ~ImpactEffect();

	void Init(const Vector3& Pos);
	void Update();
	void Draw(const ViewProjection&viewProjection);

	bool IsFinished() const;

};
