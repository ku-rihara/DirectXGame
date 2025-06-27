#include "MonsterBall.h"
#include "assert.h"
#include"Lighrt/Light.h"
#include"Frame/Frame.h"
#include"input/input.h"
#include<imgui.h>

MonsterBall::MonsterBall() {}

MonsterBall::~MonsterBall() {}

void MonsterBall::Init() {
    objct3D_.reset(Object3d::CreateModel("FieldFence.obj"));
	transform_.Init();
	transform_.translation_.y = -5.0f;
	transform_.translation_.z = -14.0f;
	transform_.scale_ = {1, 1, 1};

	// イージングセッティング
	//easing_.SettingValue(EasingType::OutBack, Vector3::ZeroVector(), Vector3::UnitVector(), 0.5f);
    easing_.SetAdaptValue(&transform_.scale_);
	
	objct3D_->material_.materialData_->enableLighting = 3;
  
}

void MonsterBall::Update() {

	


	transform_.UpdateMatrix();
}

void MonsterBall::Draw(ViewProjection& viewProjection) {
	objct3D_->Draw(transform_, viewProjection); 
}

void  MonsterBall::Debug() {
#ifdef _DEBUG
	if (ImGui::CollapsingHeader("MonsterBall")) {
		ImGui::PushID("MonsterBall");
		ImGui::DragFloat3("Position", &transform_.translation_.x, 0.1f);
		ImGui::DragFloat3("Scale", &transform_.scale_.x, 0.1f);
		objct3D_->material_.DebugImGui();
		ImGui::PopID();
	}
#endif // _DEBUG
}

void MonsterBall::ScaleEasing() {
    easing_.Update(Frame::DeltaTime());
}