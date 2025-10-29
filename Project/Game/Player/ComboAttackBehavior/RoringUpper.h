#pragma once

#include"BaseComboAattackBehavior.h"
#include"Easing/Easing.h"

class RailManager;
class RoringUpper : public BaseComboAattackBehavior {

public:
    // コンストラクタ
    RoringUpper(Player* player);
    ~RoringUpper();

    /// 更新、デバッグ
    void Init() override;
    void Update() override;
    void Debug() override;

    void CollisionInit();
    void RailInit();
    void HitStopUpdate();
    void ChangeOrder();
    void AnimationMove();

	void EasingInit();

private:
	enum class Order {
		UPPER,
		WAIT,
		END,
	};
private:

	/// ===================================================
	/// private varians
	/// ===================================================

	Order order_;              
	RailManager* railManager_;
	
    float kCollisionAliveTime_;
	float fallInitSpeed_;
	Vector3 forwardDirection_;

	/// パンチ
  
	float waitTine_;         
	Vector3 initPos_;

	///反動
	float jumpPower_;
	float gravity_;
	float fallSpeedLimit_;
	Vector3 tempWorldPos_;
	Vector3 backlashPos_;
	Easing<Vector3> backlashEase_;

	Easing<float> rotateEase_; 
	float xRotate_;
	
	//hitStop
	bool isHitStop_;
	

	float collisionAdaptTime_;

};
