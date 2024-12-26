#include"PunchCollisionSecond.h"
#include<imgui.h>

void PunchCollisionSecond::Init() {
	BaseCollisionBox::Init();
}

void PunchCollisionSecond::Update() {
	BaseCollisionBox::Update();
}

void PunchCollisionSecond::Draw() {
	BaseCollisionBox::Draw();
}


Vector3 PunchCollisionSecond::GetCollisionPos() const {
	return BaseCollisionBox::GetCollisionPos();
}