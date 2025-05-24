#include "Field.h"

#include "assert.h"


Vector3 Field::baseScale_ = {70,1,70};

Field::Field() {}

Field::~Field() {}

void Field::Init() {

	obj3d_.reset(Object3d::CreateModel("Field",".obj"));
	
	transform_.Init();
	transform_.scale_ = {1,1,1};
	obj3d_->material_.materialData_->enableLighting = 5;
    obj3d_->material_.SetEnvironmentCoefficient(0.35f);
}

void Field::Update() {
	transform_.UpdateMatrix();
}


void Field::Draw(ViewProjection& viewProjection) {
	obj3d_->Draw(transform_, viewProjection);
}
