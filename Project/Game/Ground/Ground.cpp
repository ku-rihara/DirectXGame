#include "Ground.h"
#include "assert.h"
#include"Lighrt/Light.h"

Ground::Ground() {}

Ground::~Ground() {}

void Ground::Init() {
	objct3D_.reset(Object3d::CreateModel("Ground",".obj"));
	transform_.Init();
	transform_.translation_.y = -10.0f;
	transform_.scale_ = {1, 2, 1};
	
	objct3D_->material_.materialData_->enableLighting = 6;
}

void Ground::Update() {
	transform_.UpdateMatrix();
}

void Ground::Draw(ViewProjection& viewProjection) {
	objct3D_->Draw(transform_, viewProjection); 
}
