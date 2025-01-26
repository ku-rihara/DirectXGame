#include "Ground.h"
#include "assert.h"
#include"Lighrt/Light.h"
#include<imgui.h>

Ground::Ground() {}

Ground::~Ground() {}

void Ground::Init() {
	objct3D_.reset(Object3d::CreateModel("terrain",".obj"));
	transform_.Init();
	transform_.translation_.y = -10.0f;
	transform_.scale_ = {1, 1, 1};
	
    lightMode_ = Mode::POINT;
}

void Ground::Update() {
    
	transform_.UpdateMatrix();
}

void Ground::Draw(ViewProjection& viewProjection) {
	objct3D_->Draw(transform_, viewProjection); 
}

void  Ground::Debug() {
    if (ImGui::CollapsingHeader("Ground")) {
		objct3D_->material_.DebugImGui();
    }
}