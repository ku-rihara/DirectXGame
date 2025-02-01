#pragma once
#include"GameCamera/Behavior/BaseGameCameraBehavior.h"

#include "3d/ViewProjection.h"
#include "3d/WorldTransform.h"
#include <memory>


class LockOn;
class GameCamera {
private:

	/// ===================================================
	/// private varius
	/// ===================================================

	LockOn* lockOn_;
	std::unique_ptr<BaseGameCameraBehavior>behavior_;
	ViewProjection viewprojection_;

	const   WorldTransform* target_ = nullptr;                 /// 追従対象
	Vector3 stickInput_;                                       /// スティック入力ベクトル
	Vector3 interTarget_ = {};                                 /// 
	float   destinationAngleY_;                                /// Y軸回転
	int     viewMoveTime_;                                     /// カメラ移動時間

	Vector3 shakePos_;
	float   rotate_;                                           /// 回転

	float   firstRotate_;
	float   zoomORotate_;
	Vector3 firstOffset_;
	Vector3 zoomOffset_;
	Vector3 offset_;                                           /// オフセット

public:
	/// ===================================================
	/// public method
	/// ===================================================
	void Init();                                              /// 初期化
	void Update();                                            /// 更新
	void MoveUpdate();                                        /// カメラ移動更新
	void Reset();                                             /// リセット
	void GetIsCameraMove();                                   /// カメラ動いているか

	void Debug();

	void RotateAdapt();
	void TranslateAdapt();

	Vector3 OffsetCalc(const Vector3& offset)const;
	Vector3 GetWorldPos() const;

	void ChangeShakeMode();
	void ChangeBehavior(std::unique_ptr<BaseGameCameraBehavior>behavior);

	/// ===================================================
	/// getter
	/// ===================================================
	const ViewProjection& GetViewProjection() { return viewprojection_; }
	Vector3 GetTargetPos()const;
	float   GetFirstRoatate()const { return firstRotate_; }
	float   GetZoomRoatate()const { return zoomORotate_; }
	Vector3 GetFirstOffset()const { return firstOffset_; }
	Vector3 GetZoomOffset()const { return zoomOffset_; }
	/// ===================================================
	/// setter
	/// ===================================================
	void SetTarget(const WorldTransform* target);
	void SetRotate(const float& rotate) { rotate_ = rotate; }
	void SetOffSet(const Vector3& offset) { offset_ = offset; }
	void SetLockOn(LockOn* lockon) { lockOn_ = lockon; }
	void SetShakePos(const Vector3& shake) { shakePos_ = shake; }
	void SetDestinationAngleY_(float angle) { destinationAngleY_ = angle; }
	void SetViewProjectionPos(Vector3 pos) { viewprojection_.translation_ = pos; }

};
