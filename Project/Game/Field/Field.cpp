#include "Field.h"

Vector3 Field::baseScale_ = {70, 1, 70};


void Field::Init() {

    obj3d_.reset(Object3d::CreateModel("Field.obj"));

    obj3d_->transform_.scale_                       = {1, 1, 1};
    obj3d_->material_.materialData_->enableLighting = 5;
    obj3d_->material_.SetEnvironmentCoefficient(0.35f);

 
}

void Field::Update() {

  
}
