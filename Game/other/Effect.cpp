#include "Effect.h"
#include"Easing.h"

void Effect::Init(Model* model, const Vector3& Pos) { 
	model_ = model;
	worldTransform_.translation_ = Pos;
	easeT_ = 0.0f;
	//model_->SetAlpha(0.5f);
	isFinished_ = false;
	worldTransform_.Init();
}

void Effect::Update() {
	easeT_ += 0.05f;

	if (easeT_ >= 1.0f) {
		easeT_ = 1.0f;
		isFinished_ = true;
	}
	/*model_->SetAlpha(Lerp(0.5f, 0.0f, easeT_));*/
	worldTransform_.scale_ = Lerp({0.5f,0.5f,0.5f}, Vector3(3.5f, 3.5f, 3.5f), easeT_);
	worldTransform_.UpdateMatrix();
}
void Effect::Draw(const ViewProjection& viewProjection) {
	
	model_->Draw(worldTransform_, viewProjection); 
}

bool Effect::IsFinished() const { return isFinished_; }