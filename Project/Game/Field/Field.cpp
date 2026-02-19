#include "Field.h"
#include "Light/LightingType.h"

Vector3 Field::baseScale_ = {70, 1, 70};

void Field::Init() {

    obj3d_.reset(KetaEngine::Object3d::CreateModel("BackObj/Field.obj"));

    obj3d_->transform_.scale_                                             = {1, 1, 1};
    obj3d_->GetModelMaterial()->GetMaterialData()->enableLighting         = static_cast<int32_t>(KetaEngine::LightingType::SpotLight);
    obj3d_->GetModelMaterial()->GetMaterialData()->environmentCoefficient = 0.35f;

    gpuParticlePlayer_.Init();
    gpuParticlePlayer_.InitEffect("fire", "Field");
}

void Field::Update() {
    gpuParticlePlayer_.Play("fire", "Field");
    gpuParticlePlayer_.Update();
}
