#include "AttackCollisionBox.h"
// enemy
#include "Enemy/BaseEnemy.h"
#include <imgui.h>
/// behavior
//#include "Enemy/Behavior/EnemyBoundDamage.h"
//#include "Enemy/Behavior/EnemyDamageRoot.h"
//#include "Enemy/Behavior/EnemyHitBackDamage.h"
//#include "Enemy/Behavior/EnemyStopDamage.h"
//#include "Enemy/Behavior/EnemyThrustDamage.h"
//#include "Enemy/Behavior/EnemyUpperDamage.h"

void AttackCollisionBox::Init() {

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

void AttackCollisionBox::OnCollisionEnter([[maybe_unused]] BaseCollider* other) {
    if (dynamic_cast<BaseEnemy*>(other)) {
        switch (attackType_) {
            /// -----------------------------------------------------------------------
            /// AttackCollisionBox::AttackType::NORMAL
            /// -----------------------------------------------------------------------
        case AttackCollisionBox::AttackType::NORMAL:

            break;
        }
    }
}

void AttackCollisionBox::OnCollisionStay([[maybe_unused]] BaseCollider* other) {
    if (dynamic_cast<BaseEnemy*>(other)) {
        switch (attackType_) {

            /// -----------------------------------------------------------------------
            /// AttackCollisionBox::AttackType::FALL
            /// -----------------------------------------------------------------------
        case AttackCollisionBox::AttackType::FALL:
          
            break;
            /// -----------------------------------------------------------------------
            /// AttackCollisionBox::AttackType::RUSH
            /// -----------------------------------------------------------------------
        case AttackCollisionBox::AttackType::RUSH:
           
            break;
            /// -----------------------------------------------------------------------
            /// AttackCollisionBox::AttackType::STOPPER
            /// -----------------------------------------------------------------------
        case AttackCollisionBox::AttackType::STOPPER:

            break;
            /// -----------------------------------------------------------------------
            /// AttackCollisionBox::AttackType::THRUST
            /// -----------------------------------------------------------------------
        case AttackCollisionBox::AttackType::THRUST:
            isSlow_ = true;
            break;
            /// -----------------------------------------------------------------------
            /// AttackCollisionBox::AttackType::UPPER
            /// -----------------------------------------------------------------------
        case AttackCollisionBox::AttackType::UPPER:
            isHitStop_ = true;
            break;
            /// -----------------------------------------------------------------------
            /// AttackCollisionBox::AttackType::COUNT
            /// -----------------------------------------------------------------------
        case AttackCollisionBox::AttackType::COUNT:
            break;
        default:
            break;
        }
    }
}
