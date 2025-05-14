#pragma once
#include"GameCamera/Behavior/BaseGameCameraBehavior.h"
#include"GameCamera/ShakeState/BaseShakeState.h"

#include "3d/ViewProjection.h"
#include "3d/WorldTransform.h"

#include"utility/ParameterEditor/GlobalParameter.h"
#include <memory>


class LockOn;
class GameCamera {
public:
 
	struct Paramater {
		Vector3 zoomOffset_;
		Vector3 firstOffset_;
		float   firstRotate_;
		float   zoomRotate_;
		float   backLashTime_;
		float   backLashRatio_;
		Vector3 backLashOffset_;
	};

private:

	/// ===================================================
	/// private varius
	/// ===================================================

	GlobalParameter* globalParameter_;            /// グローバルパラメータ
	const std::string groupName_ = "GameCamera";      /// グループ名

	LockOn* lockOn_;
	ViewProjection viewprojection_;

	std::unique_ptr<BaseGameCameraBehavior>behavior_;
	std::unique_ptr<BaseShakeState>shakeState_;
	
	const   WorldTransform* target_ = nullptr; /// 追従対象
	Vector3 stickInput_;                       /// スティック入力ベクトル
	Vector3 interTarget_ = {};                 /// 
	float   destinationAngleY_;                /// Y軸回転
	int     viewMoveTime_;                     /// カメラ移動時間
											   
	Vector3 shakePos_;						   
	float   rotate_;                           /// 回転
											   
	Paramater paramater_;					   
											   
	Vector3 offset_;                           /// オフセット

public:
	/// ===================================================
	/// public method
	/// ===================================================
	void Init();              /// 初期化
	void Update();            /// 更新
	void MoveUpdate();        /// カメラ移動更新
	void Reset();             /// リセット
	void GetIsCameraMove();   /// カメラ動いているか判定

	void Debug();

	void RotateAdapt();
	void TranslateAdapt();

	Vector3 OffsetCalc(const Vector3& offset)const;
	Vector3 GetWorldPos() const;

	void ChangeShakeMode();
	void ChangeZoomInOut();
	void ChangeBehavior(std::unique_ptr<BaseGameCameraBehavior>behavior);
	void ChangeShakeState(std::unique_ptr<BaseShakeState>behavior);

	///-------------------------------------------------------------------------------------
///Editor
///-------------------------------------------------------------------------------------
	void ParmLoadForImGui();
	void AddParmGroup();
	void SetValues();
	void ApplyGlobalParameter();
	void AdjustParm();

	/// ===================================================
	/// getter
	/// ===================================================
	const ViewProjection& GetViewProjection() { return viewprojection_; }
	Vector3 GetTargetPos()const;
	Vector3 GetOffset()const { return offset_; };
	Paramater GetParamater()const { return paramater_; }
	/// ===================================================
	/// setter
	/// ===================================================
	void SetTarget(const WorldTransform* target);
	void SetRotate(const float& rotate) { rotate_ = rotate; }
	void SetOffSet(const Vector3& offset) { offset_ = offset; }
	void SetLockOn(LockOn* lockon) { lockOn_ = lockon; }
	void SetShakePos(const Vector3& shake) { shakePos_ = shake; }
	void SetShakePosY(const float& shake) { shakePos_.y = shake; }
	void SetDestinationAngleY_(float angle) { destinationAngleY_ = angle; }
	void SetViewProjectionPos(Vector3 pos) { viewprojection_.translation_ = pos; }

};
