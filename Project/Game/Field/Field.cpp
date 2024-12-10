#include "Field.h"

#include "assert.h"


Vector3 Field::baseScale_ = {70,1,70};

Field::Field() {}

Field::~Field() {}

void Field::Init() {

	obj3d_.reset(Object3d::CreateModel("Field/Field",".obj"));
	
	transform_.Init();
	transform_.scale_ = baseScale_;
}

void Field::Update() {
	transform_.UpdateMatrix();
}


void Field::Draw(ViewProjection& viewProjection) {
	obj3d_->Draw(transform_, viewProjection);
}
