#include "Skydome.h"


SkyDome::SkyDome() {}

SkyDome::~SkyDome() {}

void SkyDome::Init() {
    obj3D_.reset(KetaEngine::Object3d::CreateModel("SkyDome.obj"));
	transform_.Init();
	transform_.scale_ = { 200, 200, 200 };
}

void SkyDome::Update() {
	transform_.UpdateMatrix();
}
