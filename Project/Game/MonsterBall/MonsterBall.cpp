#include "MonsterBall.h"
#include "assert.h"
#include "Frame/Frame.h"
#include "input/input.h"
#include "Lighrt/Light.h"
#include <imgui.h>

MonsterBall::MonsterBall() {}

MonsterBall::~MonsterBall() {}

void MonsterBall::Init() {

    // Animation test

    transform_.Init();
    obj3D_.reset(Object3d::CreateModel("suzanne.obj"));

    modelAnimation_ = std::make_unique<ModelAnimation>();
    modelAnimation_->Create("WalkAnimation.gltf");
    modelAnimation_->Add("Walk.gltf");

    modelAnimation2_ = std::make_unique<ModelAnimation>();
    modelAnimation2_->Create("Walk.gltf");

  
    modelAnimation_->transform_.translation_.y = -5.0f;
    modelAnimation_->transform_.translation_.z = -14.0f;
    modelAnimation_->transform_.scale_         = {1, 1, 1};

  
    modelAnimation2_->transform_.translation_.y = -5.0f;
    modelAnimation2_->transform_.translation_.z = -14.0f;
    modelAnimation2_->transform_.scale_         = {1, 1, 1};

    emitter_.reset(ParticleEmitter::CreateParticlePrimitive("jointTest", PrimitiveType::Plane, 300));

    emitter_->SetParentJoint(modelAnimation_.get(), "mixamorig:RightHand");
    transform_.SetParentJoint(modelAnimation_.get(), "mixamorig:LeftHand");

    transform_.scale_={33, 33, 33};
    modelAnimation_->transform_.rotation_.y = 3.14f;

}

void MonsterBall::Update() {

    modelAnimation_->Update(Frame::DeltaTime());

    emitter_->Update();
    emitter_->Emit();
    emitter_->EditorUpdate();

    transform_.UpdateMatrix();
}

void MonsterBall::Draw(ViewProjection& viewProjection) {
    modelAnimation_->Draw(viewProjection);
    modelAnimation_->DebugDraw(viewProjection);

    modelAnimation2_->Draw(viewProjection);
    modelAnimation2_->DebugDraw(viewProjection);
}

void MonsterBall::Debug() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader("MonsterBall")) {
        ImGui::PushID("MonsterBall");
        ImGui::DragFloat3("Position", &modelAnimation_->transform_.translation_.x, 0.1f);
        ImGui::DragFloat3("R", &modelAnimation_->transform_.rotation_.x, 0.1f);
        ImGui::DragFloat3("Scale", &modelAnimation_->transform_.scale_.x, 0.1f);
        /*	objct3D_->material_.DebugImGui();*/
        ImGui::PopID();
    }
#endif // _DEBUG
}
