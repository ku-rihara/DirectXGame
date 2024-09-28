#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"

class LockOn;
class FollowCamera {
private:
	//包含
	LockOn* lockOn_;

	ViewProjection viewprojection_;
	// 追従対象
	const WorldTransform* target_ = nullptr;
	//追従対象の残像座標
	Vector3 interTarget_ = {};
	float destinationAngleY_;

public:
	void Init();
	void Update();
	void Reset();
	Vector3 OffsetCalc()const;

	// getter
	const ViewProjection& GetViewProjection() { return viewprojection_; }
	// setter
	void SetTarget(const WorldTransform* target);
	void SetLockOn(LockOn* lockon) { lockOn_ = lockon; }
};
