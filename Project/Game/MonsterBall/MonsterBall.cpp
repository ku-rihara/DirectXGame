#include "MonsterBall.h"
#include "assert.h"
#include "Frame/Frame.h"
#include "input/input.h"
#include "Lighrt/Light.h"
#include <imgui.h>

MonsterBall::MonsterBall() {}

MonsterBall::~MonsterBall() {}

void MonsterBall::Init() {

    transform_.Init();
    obj3D_.reset(Object3d::CreateModel("Suzanne.obj"));

    modelAnimation_ = std::make_unique<ModelAnimation>();
    modelAnimation_->Create("SneakWalk.gltf");
    modelAnimation_->Add("walk.gltf");

    modelAnimation2_ = std::make_unique<ModelAnimation>();
    modelAnimation2_->Create("walk.gltf");

    modelAnimation_->transform_.translation_.y = -5.0f;
    modelAnimation_->transform_.translation_.z = -14.0f;
    modelAnimation_->transform_.scale_         = {1, 1, 1};

    modelAnimation2_->transform_.translation_.y = -5.0f;
    modelAnimation2_->transform_.translation_.z = -14.0f;
    modelAnimation2_->transform_.scale_         = {1, 1, 1};

    // イージングセッティング
    easing_.SetAdaptValue(&transform_.scale_);

    transform_.SetParentJoint(modelAnimation_.get(), "mixamorig:LeftHand");
}

void MonsterBall::Update() {
    if (Input::GetInstance()->PushKey(DIK_E)) {
        modelAnimation_->Update(Frame::DeltaTime());
     
    }

    if (Input::GetInstance()->TrrigerKey(DIK_X)) {
        modelAnimation_->ChangeAnimation("walk");
    }

    transform_.UpdateMatrix();
}

void MonsterBall::Draw(ViewProjection& viewProjection) {
    modelAnimation_->Draw(viewProjection);
    modelAnimation_->DebugDraw(viewProjection);

    modelAnimation2_->Draw(viewProjection);
    modelAnimation2_->DebugDraw(viewProjection);

    obj3D_->Draw(transform_, viewProjection);
}

void MonsterBall::Debug() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader("MonsterBall")) {
        ImGui::PushID("MonsterBall");
        ImGui::DragFloat3("Position", &modelAnimation_->transform_.translation_.x, 0.1f);
        ImGui::DragFloat3("R", &modelAnimation_->transform_.rotation_.x, 0.1f);
        ImGui::DragFloat3("Scale", &modelAnimation_->transform_.scale_.x, 0.1f);

        ImGui::DragFloat3("MPosition", &transform_.translation_.x, 0.1f);
        ImGui::DragFloat3("MR", &transform_.rotation_.x, 0.1f);
        ImGui::DragFloat3("MScale", &transform_.scale_.x, 0.1f);
        /*	objct3D_->material_.DebugImGui();*/
        ImGui::PopID();
    }
#endif // _DEBUG
}

void MonsterBall::ScaleEasing() {
    easing_.Update(Frame::DeltaTime());
}