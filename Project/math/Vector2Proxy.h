#pragma once
#include "Vector2.h"
#include "Vector3.h"

struct IVector2Proxy {
    virtual Vector2 Get() const        = 0;
    virtual void Set(const Vector2& v) = 0;
    virtual ~IVector2Proxy()           = default;
};

struct XYProxy : public IVector2Proxy {
    float& x;
    float& y;
    XYProxy(Vector3& vec) : x(vec.x), y(vec.y) {}
    Vector2 Get() const override { return Vector2(x, y); }
    void Set(const Vector2& v) override {
        x = v.x;
        y = v.y;
    }
};

struct XZProxy : public IVector2Proxy {
    float& x;
    float& z;
    XZProxy(Vector3& vec) : x(vec.x), z(vec.z) {}
    Vector2 Get() const override { return Vector2(x, z); }
    void Set(const Vector2& v) override {
        x = v.x;
        z = v.y;
    }
};

struct YZProxy : public IVector2Proxy {
    float& y;
    float& z;
    YZProxy(Vector3& vec) : y(vec.y), z(vec.z) {}
    Vector2 Get() const override { return Vector2(y, z); }
    void Set(const Vector2& v) override {
        y = v.x;
        z = v.y;
    }
};