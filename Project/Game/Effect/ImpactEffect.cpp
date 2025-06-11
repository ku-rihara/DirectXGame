#include "ImpactEffect.h"
#include"Easing.h"
#include"MathFunction.h"
#include"Frame/Frame.h"

ImpactEffect::ImpactEffect() {

}
ImpactEffect::~ImpactEffect() {

}

void ImpactEffect::Init(const Vector3& Pos) { 
	obj3d_.reset(Object3d::CreateModel("DamageEffect.obj"));
	worldTransform_.Init();
	worldTransform_.translation_ = Pos;
	easeT_ = 0.0f;
	alpha_ = 1.0f;
	obj3d_->objColor_.SetColor(Vector4(1,0,0, alpha_));
	isFinished_ = false;
	
}

void ImpactEffect::Update() {
	easeT_ += Frame::DeltaTimeRate()*2.5f;

	if (easeT_ >= 1.0f) {
		easeT_ = 1.0f;
		isFinished_ = true;
	}
	alpha_=(Lerp(1.0f, 0.0f, easeT_));
	obj3d_->objColor_.SetColor(Vector4(1, 0, 0, alpha_));

	worldTransform_.scale_ = Lerp({0.5f,0.5f,0.5f}, Vector3(12.5f, 12.5f, 12.5f), easeT_);
	worldTransform_.UpdateMatrix();
}
void ImpactEffect::Draw(const ViewProjection& viewProjection) {
	
	obj3d_->Draw(worldTransform_, viewProjection); 
}

bool ImpactEffect::IsFinished() const { return isFinished_; }