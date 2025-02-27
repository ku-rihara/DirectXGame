#include"StarEffect.h"
#include"Easing.h"
#include"MathFunction.h"
#include"Frame/Frame.h"

void StarEffect::Init(const Vector3& Pos) {
	obj3d_.reset(Object3d::CreateModel("DamageEffect",".obj"));
	worldTransform_.Init();
	worldTransform_.translation_ = Pos;
	isFinished_ = false;
	
}

void StarEffect::Update() {
	
}
void StarEffect::Draw(const ViewProjection& viewProjection) {
	
	obj3d_->Draw(worldTransform_, viewProjection); 
}


void StarEffect::SetViewProjection(const ViewProjection* view) {
	viewProjection_ = view;
}