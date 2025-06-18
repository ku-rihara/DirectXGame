#include "Vector2Proxy.h"

// ---------- XYProxy ----------
XYProxy::XYProxy(Vector3* target) : target_(target) {
    if (target_) {
        proxy_ = Vector2(target_->x, target_->y);
    }
}

Vector2& XYProxy::Get() {
    Update();
    return proxy_;
}

const Vector2& XYProxy::Get() const {
    return proxy_;
}

void XYProxy::Update() {
    if (target_) {
        proxy_.x = target_->x;
        proxy_.y = target_->y;
    }
}

void XYProxy::Apply() {
    if (target_) {
        target_->x = proxy_.x;
        target_->y = proxy_.y;
    }
}

// ---------- XZProxy ----------
XZProxy::XZProxy(Vector3* target) : target_(target) {
    if (target_) {
        proxy_ = Vector2(target_->x, target_->z);
    }
}

Vector2& XZProxy::Get() {
    Update();
    return proxy_;
}

const Vector2& XZProxy::Get() const {
    return proxy_;
}

void XZProxy::Update() {
    if (target_) {
        proxy_.x = target_->x;
        proxy_.y = target_->z;
    }
}

void XZProxy::Apply() {
    if (target_) {
        target_->x = proxy_.x;
        target_->z = proxy_.y;
    }
}

// ---------- YZProxy ----------
YZProxy::YZProxy(Vector3* target) : target_(target) {
    if (target_) {
        proxy_ = Vector2(target_->y, target_->z);
    }
}

Vector2& YZProxy::Get() {
    Update();
    return proxy_;
}

const Vector2& YZProxy::Get() const {
    return proxy_;
}

void YZProxy::Update() {
    if (target_) {
        proxy_.x = target_->y;
        proxy_.y = target_->z;
    }
}

void YZProxy::Apply() {
    if (target_) {
        target_->y = proxy_.x;
        target_->z = proxy_.y;
    }
}
