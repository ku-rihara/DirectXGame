#include"PunchCollisionSecond.h"
#include<imgui.h>

void PunchCollisionSecond::Init() {
	BasePunchCollisionBox::Init();
}

void PunchCollisionSecond::Update() {
	BasePunchCollisionBox::Update();
}

void PunchCollisionSecond::Draw() {
	BasePunchCollisionBox::Draw();
}


Vector3 PunchCollisionSecond::GetCollisionPos() const {
	return BasePunchCollisionBox::GetCollisionPos();
}