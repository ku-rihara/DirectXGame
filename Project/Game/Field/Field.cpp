#include "Field.h"

#include "assert.h"

Vector3 Field::baseScale_ = {70, 1, 70};

Field::Field() {}

Field::~Field() {}

void Field::Init() {

    obj3d_.reset(Object3d::CreateModel("Field.obj"));

    transform_.Init();
    transform_.scale_                               = {1, 1, 1};
    obj3d_->material_.materialData_->enableLighting = 5;
    obj3d_->material_.SetEnvironmentCoefficient(0.35f);

    emitter_[0].reset(ParticleEmitter::CreateParticlePrimitive("fieldParticleFireSpark", PrimitiveType::Plane, 800));
}

void Field::Update() {

    emitter_[0]->Update();
    emitter_[0]->EditorUpdate();
    emitter_[0]->Emit();

    transform_.UpdateMatrix();
}

void Field::Draw(ViewProjection& viewProjection) {
    obj3d_->Draw(transform_, viewProjection);
}
