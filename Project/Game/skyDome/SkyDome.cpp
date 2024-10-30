#include "Skydome.h"
#include "assert.h"

Skydome::Skydome() {}

Skydome::~Skydome() {}

void Skydome::Init() {
	objct3D_.reset(Object3d::CreateModel("skydome", ".obj"));
	transform_.Init();
	transform_.scale_ = {200, 200, 200};
}

void Skydome::Update() { 
	transform_.UpdateMatrix();
}

void Skydome::Draw(ViewProjection& viewProjection) {
	objct3D_->Draw(transform_, viewProjection); 
}
