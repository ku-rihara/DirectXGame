#pragma once

#include"BasePlayerBehavior.h"
#include"Easing/Easing.h"
#include"Easing/EasingSequence.h"

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
    std::unique_ptr<EasingSequence> moveEase_;
    float tempWaitScaleY_;
    float moveEaseTime_;

    float tempPosY_ = 0.0f;
    Vector3 tempScale_;
};
