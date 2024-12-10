#pragma once
#include "3d/ViewProjection.h"
#include "3d/WorldTransform.h"

//#include"GameCameraBehavior/BaseGameCameraBehavior.h"
class LockOn;
class GameCamera {
private:
	/// ===================================================
	///private varius
	/// ===================================================
//包含
	LockOn* lockOn_;

	ViewProjection viewprojection_;
	// 追従対象
	const WorldTransform* target_ = nullptr;
	//追従対象の残像座標
	Vector3 stickInput_;
	Vector3 interTarget_ = {};
	float destinationAngleY_;
	int viewMoveTime_;
	const int kViewMoveTime_ = 20;

public:
	/// ===================================================
	///public method
	/// ===================================================
	void Init();
	void Update();
	void Reset();
	void GetIsCameraMove();
	Vector3 OffsetCalc()const;

	Vector3 GetBaseCenterPosition() const;

	/// ===================================================
	/// getter
	/// ===================================================
	const ViewProjection& GetViewProjection() { return viewprojection_; }
		Vector3 GetTargetPos()const;
	/// ===================================================
	/// setter
	/// ===================================================
	void SetTarget(const WorldTransform* target);
	void SetLockOn(LockOn* lockon) { lockOn_ = lockon; }
	void SetDestinationAngleY_(float angle) { destinationAngleY_ = angle; }
	void SetViewProjectionPos(Vector3 pos) { viewprojection_.translation_ = pos; }
	
};
