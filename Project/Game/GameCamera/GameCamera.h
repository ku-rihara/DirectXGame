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
	bool isTutorialViewMove_;

public:
	/// ===================================================
	///public method
	/// ===================================================
	void Init();
	void Update();
	void Reset();
	void TutorialViewMove();
	Vector3 OffsetCalc()const;

	// getter
	const ViewProjection& GetViewProjection() { return viewprojection_; }
	bool GetIsTutorialViewMove()const { return isTutorialViewMove_; }

	Vector3 GetBaseCenterPosition() const;

	/// ===================================================
	/// getter
	/// ===================================================
	const ViewProjection& GetViewProjection() { return viewprojection_; }
		Vector3 GetTargetPos()const;
	Vector3 GetBaseCenterPosition() const;
	/// ===================================================
	/// setter
	/// ===================================================
	void SetTarget(const WorldTransform* target);
	void SetLockOn(LockOn* lockon) { lockOn_ = lockon; }
	void SetDestinationAngleY_(float angle) { destinationAngleY_ = angle; }
	void SetViewProjectionPos(Vector3 pos) { viewprojection_.translation_ = pos; }
	
};
