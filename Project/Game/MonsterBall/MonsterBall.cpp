#include "MonsterBall.h"
#include "assert.h"
#include"Lighrt/Light.h"
#include<imgui.h>

MonsterBall::MonsterBall() {}

MonsterBall::~MonsterBall() {}

void MonsterBall::Init() {
	objct3D_.reset(Object3d::CreateModel("MonsterBall",".obj"));
	transform_.Init();
	transform_.translation_.y = -5.0f;
	transform_.translation_.z = -14.0f;
	transform_.scale_ = {1, 1, 1};
	
	objct3D_->material_.materialData_->enableLighting = 3;
  
}

void MonsterBall::Update() {
    
	transform_.UpdateMatrix();
}

void MonsterBall::Draw(ViewProjection& viewProjection) {
	objct3D_->Draw(transform_, viewProjection); 
}

void  MonsterBall::Debug() {
    if (ImGui::CollapsingHeader("MonsterBall")) {
		ImGui::PushID("MonsterBall");
		ImGui::DragFloat3("Position", &transform_.translation_.x,0.1f);
		ImGui::DragFloat3("Scale", &transform_.scale_.x, 0.1f);
		objct3D_->material_.DebugImGui();
		ImGui::PopID();
    }
}