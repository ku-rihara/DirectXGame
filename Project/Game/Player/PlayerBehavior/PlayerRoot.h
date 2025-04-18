#pragma once

#include"BasePlayerBehavior.h"
#include"Easing.h"

class PlayerRoot : public BasePlayerBehavior {
private:
	enum class AnimationStep {
		INIT,
		UPDATE,
		COOLING,
	};

private:

/// ===================================================
///private varians
/// ===================================================

	AnimationStep animationStep_;
	float easeDirection_;
	float speed_;

	float animationCollTime_;
	Easing waitEase_;
    Easing moveEase_;
    float moveEaseTime_;
	
public:
	//コンストラクタ
	PlayerRoot(Player* boss);
	~PlayerRoot();

	void MoveAnimation();
    void WaitAnimation();
	void Update()override;
	void Debug()override;

	void JumpForJoyState();

};
