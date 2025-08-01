#include "Skydome.h"


Skydome::Skydome() {}

Skydome::~Skydome() {}

void Skydome::Init() {
    obj3D_.reset(Object3d::CreateModel("SkyDome.obj"));
	transform_.Init();
	transform_.scale_ = { 200, 200, 200 };
}

void Skydome::Update() {
	transform_.UpdateMatrix();
}
