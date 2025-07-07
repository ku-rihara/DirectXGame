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
    modelAnimation_->Create("simpleSkin.gltf");

    transform_.Init();
    transform_.translation_.y = -5.0f;
    transform_.translation_.z = -14.0f;
    transform_.scale_         = {1, 1, 1};

    // イージングセッティング
    easing_.SetAdaptValue(&transform_.scale_);
}

void MonsterBall::Update() {
    if (Input::GetInstance()->PushKey(DIK_E)) {
        modelAnimation_->Update(Frame::DeltaTime());
    }

    transform_.UpdateMatrix();
}

void MonsterBall::Draw(ViewProjection& viewProjection) {
    modelAnimation_->Draw(transform_, viewProjection);
    modelAnimation_->DebugDraw(transform_, viewProjection);
}

void MonsterBall::Debug() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader("MonsterBall")) {
        ImGui::PushID("MonsterBall");
        ImGui::DragFloat3("Position", &transform_.translation_.x, 0.1f);
        ImGui::DragFloat3("R", &transform_.rotation_.x, 0.1f);
        ImGui::DragFloat3("Scale", &transform_.scale_.x, 0.1f);
        /*	objct3D_->material_.DebugImGui();*/
        ImGui::PopID();
    }
#endif // _DEBUG
}

void MonsterBall::ScaleEasing() {
    easing_.Update(Frame::DeltaTime());
}