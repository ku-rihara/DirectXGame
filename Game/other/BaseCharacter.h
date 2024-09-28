#pragma once
#include"Model.h"
#include<memory>
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "vector"
#include"Colider.h"

class BaseCharacter:public Colider {
protected:
	//モデル配列データ
	std::vector<Model*> models_;
	//ワールド変換データ
	WorldTransform baseWorldTransform_;
	std::vector<std::unique_ptr<WorldTransform>> partsWorldTransforms_;

public:
	virtual void Init(const std::vector<Model*>& models);
	virtual void Update();
	virtual void Draw(const ViewProjection&viewProjection);

	virtual Vector3 GetBaseCenterPosition() const override;

	
	virtual void AnimationInit() = 0;
	virtual void AnimationUpdate() = 0;

	const WorldTransform& GetWorldTransform() { return baseWorldTransform_; }
	const std::vector<std::unique_ptr<WorldTransform>>& GetPartsWorldTransforms() { return partsWorldTransforms_; }
};
