#include"AttackCollisionBox.h"
#include"Enemy/BaseEnemy.h"
#include<imgui.h>

void AttackCollisionBox::Init() {
	/*tagList_[static_cast<size_t>(AttackType::NORMAL)] = "Normal";
	tagList_[static_cast<size_t>(AttackType::FALL)] = "Fall";
	tagList_[static_cast<size_t>(AttackType::RUSH)] = "Rush";
	tagList_[static_cast<size_t>(AttackType::THRUST)] = "Thrust";
	tagList_[static_cast<size_t>(AttackType::STOPPER)] = "Stopper";
	tagList_[static_cast<size_t>(AttackType::UPPER)] = "Upper";*/
	BaseAABBCollisionBox::Init();
}

void AttackCollisionBox::Update() {
	BaseAABBCollisionBox::Update();
}

void AttackCollisionBox::Draw() {
	BaseAABBCollisionBox::Draw();
}

Vector3 AttackCollisionBox::GetCollisionPos() const {
	return BaseAABBCollisionBox::GetCollisionPos();
}

void AttackCollisionBox::SetSize(const Vector3& size) {
	BaseAABBCollisionBox::SetSize(size);
}

void AttackCollisionBox::SetPosition(const Vector3& position) {
	BaseAABBCollisionBox::SetPosition(position);
}

void AttackCollisionBox::SetOffset(const Vector3& offset) {
	BaseAABBCollisionBox::SetOffset(offset);
}

void AttackCollisionBox::IsAdapt(bool is) {
	BaseAABBCollisionBox::IsAdapt(is);
}

void AttackCollisionBox::OnCollisionStay([[maybe_unused]] BaseCollider* other) {
	if (dynamic_cast<BaseEnemy*>(other)) {
		switch (attackType_)
		{
		case AttackCollisionBox::AttackType::NORMAL:
			break;
		case AttackCollisionBox::AttackType::FALL:
			break;
		case AttackCollisionBox::AttackType::RUSH:
			break;
		case AttackCollisionBox::AttackType::STOPPER:
			break;
		case AttackCollisionBox::AttackType::THRUST:
			isSlow_ = true;
			break;
		case AttackCollisionBox::AttackType::UPPER:
			isHitStop_ = true;
			break;
		case AttackCollisionBox::AttackType::COUNT:
			break;
		default:
			break;
		}
		
	}
}
