#include "SphereCollider.h"

using namespace KetaEngine;
#include "Collider/CollisionManager.h"
#include<numbers>
#include <cmath>

SphereCollider::SphereCollider() {
    CollisionManager::AddCollider(this);
}

SphereCollider::~SphereCollider() {
    CollisionManager::RemoveCollider(this);
}

void SphereCollider::Init() {
    // 3円 × 16分割 = 48本のライン
    debugLine_.reset(Line3D::Create(48));
    cTransform_.Init();
    sphere_.radius = 1.5f;
}

void SphereCollider::UpdateWorldTransform() {
    sphere_.center = GetCollisionPos();

    cTransform_.translation_ = sphere_.center;
    cTransform_.UpdateMatrix();
}

void SphereCollider::SetDebugCube() {
    const Vector3& c = sphere_.center;
    const float r = sphere_.radius;
    const int segments = 16;

    // XZ平面（水平円）
    for (int i = 0; i < segments; i++) {
        float a0 = (2.0f * std::numbers::pi_v<float> * i) / segments;
        float a1 = (2.0f * std::numbers::pi_v<float> * (i + 1)) / segments;
        Vector3 p0 = { c.x + r * std::cos(a0), c.y, c.z + r * std::sin(a0) };
        Vector3 p1 = { c.x + r * std::cos(a1), c.y, c.z + r * std::sin(a1) };
        debugLine_->SetLine(p0, p1, lineColor_);
    }

    // XY平面
    for (int i = 0; i < segments; i++) {
        float a0 = (2.0f * std::numbers::pi_v<float> * i) / segments;
        float a1 = (2.0f * std::numbers::pi_v<float> * (i + 1)) / segments;
        Vector3 p0 = { c.x + r * std::cos(a0), c.y + r * std::sin(a0), c.z };
        Vector3 p1 = { c.x + r * std::cos(a1), c.y + r * std::sin(a1), c.z };
        debugLine_->SetLine(p0, p1, lineColor_);
    }

    // YZ平面
    for (int i = 0; i < segments; i++) {
        float a0 = (2.0f * std::numbers::pi_v<float> * i) / segments;
        float a1 = (2.0f * std::numbers::pi_v<float> * (i + 1)) / segments;
        Vector3 p0 = { c.x, c.y + r * std::sin(a0), c.z + r * std::cos(a0) };
        Vector3 p1 = { c.x, c.y + r * std::sin(a1), c.z + r * std::cos(a1) };
        debugLine_->SetLine(p0, p1, lineColor_);
    }
}

Vector3 SphereCollider::GetCollisionPos() const {
    return BaseCollider::GetCollisionPos();
}
