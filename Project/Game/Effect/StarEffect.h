#pragma once
#include"3d/WorldTransform.h"
#include"3d/ViewProjection.h"
#include "3d/Object3d.h"
class StarEffect {
public:
	enum class Step {
		INIT,
		UPDATE
	};
private:
	///=====================================================================
	/// private variant
	///=====================================================================
	std::unique_ptr<Object3d> obj3d_;
	WorldTransform worldTransform_;
	const ViewProjection* viewProjection_;
	bool isFinished_;

	StarEffect* pfuncTable_;
	
public:

	///=====================================================================
	/// public method
	///=====================================================================
	void Init(const Vector3& Pos);
	void Update();
	void Draw(const ViewProjection&viewProjection);

	///=====================================================================
	/// getter method
	///=====================================================================
	bool GetIsFinished()  const { return isFinished_; }

	///=====================================================================
	/// setter method
	///=====================================================================
	void SetViewProjection(const ViewProjection*view);
};
