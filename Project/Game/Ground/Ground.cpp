#include "Ground.h"
#include "assert.h"
#include"Lighrt/Light.h"
#include<imgui.h>

Ground::Ground() {}

Ground::~Ground() {}

void Ground::Init() {
    objct3D_.reset(Object3d::CreateModel("terrain.obj"));
	transform_.Init();
	transform_.translation_.y = -1.0f;
	transform_.scale_ = {1, 1, 1};  
}

void Ground::Update() {
    
	transform_.UpdateMatrix();
}


void  Ground::Debug() {
#ifdef _DEBUG

    if (ImGui::CollapsingHeader("Ground")) {
		ImGui::PushID("Ground");
		ImGui::DragFloat3("Position", &transform_.translation_.x, 0.1f);
		ImGui::DragFloat3("Scale", &transform_.scale_.x, 0.1f);
		objct3D_->material_.DebugImGui();
		ImGui::PopID();
    }
#endif // _DEBUG
}