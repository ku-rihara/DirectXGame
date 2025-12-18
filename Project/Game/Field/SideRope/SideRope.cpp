#include "SideRope.h"

void SideRope::Init() {
}

void SideRope::Update() {
}

void SideRope::Rebound(const ReboundTarget& target) {
    target;
}

Vector3 SideRope::GetCollisionPos() const {

    // ワールド座標に変換
    Vector3 worldPos = TransformMatrix(Vector3::ZeroVector(), transform_.matWorld_);
    return worldPos;
}
