#pragma once

#include"BasePlayerBehavior.h"
#include"Easing.h"

class PlayerMove : public BasePlayerBehavior {
public:
    // コンストラクタ
    PlayerMove(Player* player);
    ~PlayerMove();

    void MoveAnimation();
    void WaitAnimation();
    void Update() override;
    void Debug() override;

    void JumpForJoyState();

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
	float speed_;

	float animationCollTime_;
	Easing<float> waitEase_;
    float tempWaitScaleY_;
  /*  Easing moveEase_;*/
    float moveEaseTime_;
	

};
