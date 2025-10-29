#pragma once

#include"BaseComboAattackBehavior.h"
#include"Easing/Easing.h"

class RailManager;
class JumpRush : public BaseComboAattackBehavior {
private:
	enum class Order {
		CALUCRATION,
		RUSH,
		NEXTBEHAVIOR,
	};
private:

	/// ===================================================
	///private varians
	/// ===================================================

	Order order_;

	Vector3 forwardDirection_;

	Vector3 initPos_;
    Vector3 preWorldPos_;
    Vector3 frontPos_;

	Easing<float> assaultJumpEase_;
    float tempPositionY_;

	Easing<Vector3> assaultEase_;
    Vector3 tempPosition_;

public:
	//コンストラクタ
    JumpRush(Player* player);
	~JumpRush();

	/// 更新、デバッグ
    void Init() override;
	void Update()override;
	void Debug()override;

};
