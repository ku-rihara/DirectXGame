#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"

#include"Game/GameCameraBehavior/BaseGameCameraBehavior.h"
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
	const int kViewMoveTime_=20;
	bool isSideAttackCamera_;
	/// behavior
	bool isBackToRootMode_;
	std::unique_ptr<BaseGameCameraBehavior>behavior_=nullptr;

public:
	/// ===================================================
	///public method
	/// ===================================================
	void Init();
	void Update();
	void Reset();

	void RootUpdate();
	void TutorialViewMove();

	///　behavior
	void CameraBackInit();
	void CameraBackUpdate();

	void Debug();
	Vector3 OffsetCalc()const;

	///　振る舞い切り替え
	void SetBackToRootFrag();
	void SetBehaviorBack();

	void ChangeBehavior(std::unique_ptr<BaseGameCameraBehavior>behavior);

	/// ===================================================
	/// getter
	/// ===================================================
	const ViewProjection& GetViewProjection() { return viewprojection_; }
	bool GetIsSideAttackCamera()const { return isSideAttackCamera_; }
	bool GetIsBackToRootMode()const { return isBackToRootMode_; }
	Vector3 GetTargetPos()const;
	Vector3 GetBaseCenterPosition() const;
	/// ===================================================
	/// setter
	/// ===================================================
	void SetTarget(const WorldTransform* target);
	void SetIsSideAttackCamera(bool is) { isSideAttackCamera_ = is; }
	void SetIsBackToRootMode(bool is) { isBackToRootMode_ = is; }
	void SetLockOn(LockOn* lockon) { lockOn_ = lockon; }
	void SetDestinationAngleY_(float angle) { destinationAngleY_ = angle; }
	void SetViewProjectionPos(Vector3 pos) { viewprojection_.translation_ = pos; }
	
};
