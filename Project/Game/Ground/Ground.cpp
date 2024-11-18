#include "Ground.h"
#include "assert.h"

Ground::Ground() {}

Ground::~Ground() {}

void Ground::Init() {
	objct3D_.reset(Object3d::CreateModel("Ground",".obj"));
	transform_.Init();
	transform_.translation_.y = -60.0f;
	transform_.scale_ = {800, 2, 800};
	transform_.isNotViewRestriction_ = true;
}

void Ground::Update() {
	transform_.UpdateMatrix();
}

void Ground::Draw(ViewProjection& viewProjection) {
	objct3D_->Draw(transform_, viewProjection); 
}
