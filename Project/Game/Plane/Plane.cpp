#include "Plane.h"
#include "assert.h"
#include"Lighrt/Light.h"
#include<imgui.h>

Plane::Plane() {}

Plane::~Plane() {}

void Plane::Init() {

	/// .obj
	objct3D_.reset(Object3d::CreateModel("Suzanne.obj"));
	transform_.Init();
	transform_.translation_ = {-0.0f,1.5f,-0.0f};
	transform_.scale_ = {1, 1, 1};

	/// .gltf
	gobjct3D_.reset(Object3d::CreateModel("MonsterBall.obj"));
	gtransform_.Init();
    gtransform_.translation_ = {5.0f, 1.5f, -0.0f};
	gtransform_.scale_ = { 1, 1, 1 };
	
	/// rotate
	transform_.rotation_ .y= 3.14f;
	/*gtransform_.rotation_.y = 3.14f;*/

	/// lighting
	objct3D_->material_.materialData_->enableLighting = 0;
	gobjct3D_->material_.materialData_->enableLighting = 0;
}

void Plane::Update() {
    
	/// updateMatrix
	transform_.UpdateMatrix();
	gtransform_.UpdateMatrix();
}

//void Plane::Draw(ViewProjection& viewProjection) {
//	objct3D_->Draw(transform_, viewProjection); 
//	gobjct3D_->Draw(gtransform_, viewProjection);
//}
//
//void Plane::Draws(ViewProjection& viewProjection) {
//    viewProjection;
//    objct3D_->ShadowDraw(transform_, viewProjection);
//    gobjct3D_->ShadowDraw(gtransform_, viewProjection);
//}


void  Plane::Debug() {
#ifdef _DEBUG
	/// .obj
    if (ImGui::CollapsingHeader("Suzanne")) {
		ImGui::PushID("Suzanne");
        ImGui::DragFloat3("Position", &objct3D_->transform_.translation_.x, 0.1f);
        ImGui::DragFloat3("Scale", &objct3D_->transform_.scale_.x, 0.1f);
		objct3D_->material_.DebugImGui();
		ImGui::PopID();
    }

	/// .gltf
	if (ImGui::CollapsingHeader("Monster")) {
		ImGui::PushID("Monster");
        ImGui::DragFloat3("Position", &gobjct3D_->transform_.translation_.x, 0.1f);
        ImGui::DragFloat3("Scale", &gobjct3D_->transform_.scale_.x, 0.1f);
		gobjct3D_->material_.DebugImGui();
		ImGui::PopID();
	}
#endif // _DEBUG
}