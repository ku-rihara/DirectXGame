#include"SideRopeReboundSystem.h"
#include"Frame/Frame.h"

void SideRopeReboundSystem::Init(
	const Vector3& initialVelocity,
	const Vector3& ropeNormal,
	float maxStretch,
	float stretchResistance,
	float reboundRate,
	float delayTime) {

	currentVelocity_   = initialVelocity;
	ropeNormal_        = ropeNormal.Normalize();
	maxStretch_        = maxStretch;
	stretchResistance_ = stretchResistance;
	reboundRate_       = reboundRate;
	delayTime_         = delayTime;

	stretchDistance_ = 0.0f;
	currentDelay_   = 0.0f;
	phase_     = Phase::STRETCH;
	isActive_  = true;

	updateFunc_ = [this](float deltaTime) {
       return UpdateStretch(deltaTime);
    };
}

Vector3 SideRopeReboundSystem::Update(float deltaTime) {

	if (!isActive_) {
        return Vector3::ZeroVector();
	}

	return updateFunc_(deltaTime);
}

Vector3 SideRopeReboundSystem::UpdateStretch(float deltaTime) {

	// ロープ方向への速度成分を取得
    float velocityAlongRope = currentVelocity_.Dot(ropeNormal_);
}