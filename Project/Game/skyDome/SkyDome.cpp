#include "Skydome.h"
#include "assert.h"

Skydome::Skydome() {}

Skydome::~Skydome() {}

void Skydome::Init() {
	objct3D_.reset(Object3d::CreateModel("skydome",".obj"));
	transform_.Init();
	transform_.scale_ = {500, 500, 500};
	transform_.isNotViewRestriction_ = true;
}

void Skydome::Update() { 
	transform_.UpdateMatrix();
}

void Skydome::Draw(ViewProjection& viewProjection) {
	objct3D_->Draw(transform_, viewProjection); 
}
