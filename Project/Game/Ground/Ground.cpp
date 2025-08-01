#include "Ground.h"
#include "assert.h"
#include"Lighrt/Light.h"
#include<imgui.h>

Ground::Ground() {}

Ground::~Ground() {}

void Ground::Init() {
    objct3D_.reset(Object3d::CreateModel("terrain.obj"));
    objct3D_->transform_.Init();
    objct3D_->transform_.translation_.y = -1.0f;
    objct3D_->transform_.scale_         = {1, 1, 1};
}

void Ground::Update() {
    
	/*objct3D_->Update();*/
}


void  Ground::Debug() {
#ifdef _DEBUG

    if (ImGui::CollapsingHeader("Ground")) {
		ImGui::PushID("Ground");
        ImGui::DragFloat3("Position", &objct3D_->transform_.translation_.x, 0.1f);
        ImGui::DragFloat3("Scale", &objct3D_->transform_.scale_.x, 0.1f);
		objct3D_->material_.DebugImGui();
		ImGui::PopID();
    }
#endif // _DEBUG
}