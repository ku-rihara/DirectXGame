#include"PunchCollisionFirst.h"
#include<imgui.h>

void PunchCollisionFirst::Init() {
	BasePunchCollisionBox::Init();
}

void PunchCollisionFirst::Update() {
	BasePunchCollisionBox::Update();
}

void PunchCollisionFirst::Draw() {
	BasePunchCollisionBox::Draw();
}


Vector3 PunchCollisionFirst::GetCollisionPos() const {
	return BasePunchCollisionBox::GetCollisionPos();
}