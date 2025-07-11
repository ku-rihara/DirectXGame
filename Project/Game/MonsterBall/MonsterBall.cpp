#include "MonsterBall.h"
#include "assert.h"
#include "Frame/Frame.h"
#include "input/input.h"
#include "Lighrt/Light.h"
#include <imgui.h>

MonsterBall::MonsterBall() {}

MonsterBall::~MonsterBall() {}

void MonsterBall::Init() {
    modelAnimation_ = std::make_unique<ModelAnimation>();
    modelAnimation_->Create("SneakWalk.gltf");

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
}

void MonsterBall::Update() {
    if (Input::GetInstance()->PushKey(DIK_E)) {
        modelAnimation_->Update(Frame::DeltaTime());
        modelAnimation2_->Update(Frame::DeltaTime());
    }

    transform_.UpdateMatrix();
    transform2_.UpdateMatrix();
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

void MonsterBall::ScaleEasing() {
    easing_.Update(Frame::DeltaTime());
}