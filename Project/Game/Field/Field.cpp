#include "Field.h"

Vector3 Field::baseScale_ = {70, 1, 70};

void Field::Init() {

    obj3d_.reset(KetaEngine::Object3d::CreateModel("Field.obj"));

    obj3d_->transform_.scale_                       = {1, 1, 1};
    obj3d_->material_.materialData_->enableLighting = 5;
    obj3d_->material_.SetEnvironmentCoefficient(0.35f);

 
    gpuParticlePlayer_.Init();
    gpuParticlePlayer_.InitEffect("Field", "fire");
}

void Field::Update() {
    gpuParticlePlayer_.Play("Field", "fire");
    gpuParticlePlayer_.Update();
}
