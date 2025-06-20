#include "EasingTestObj.h"
#include "assert.h"
#include "Frame/Frame.h"
#include "input/input.h"
#include "Lighrt/Light.h"
#include <imgui.h>

EasingTestObj::EasingTestObj() {}

EasingTestObj::~EasingTestObj() {}

void EasingTestObj::Init() {
    object3D_.reset(Object3d::CreateModel("DebugSphere.obj"));
    transform_.Init();
    transform_.translation_.y = -5.0f;
    transform_.translation_.z = -24.0f;
    transform_.scale_         = {1, 1, 1};


    easing_.Init("testEasing");
    easing_.SetAdaptValue(&transform_.scale_);
    object3D_->material_.materialData_->enableLighting = 3;
}

void EasingTestObj::Update() {

    if (Input::GetInstance()->TrrigerKey(DIK_U)) {
        easing_.Reset();
    }

    ScaleEasing();

    transform_.UpdateMatrix();
}

void EasingTestObj::Draw(ViewProjection& viewProjection) {
    object3D_->Draw(transform_, viewProjection);
}

void EasingTestObj::Debug() {
#ifdef _DEBUG
    easing_.ApplyForImGui();
#endif // _DEBUG
}

void EasingTestObj::ScaleEasing() {
    easing_.Update(Frame::DeltaTime());
}