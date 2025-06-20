#pragma once
#include "Vector2.h"
#include "Vector3.h"

class IVector2Proxy {
public:
    virtual ~IVector2Proxy() = default;

    virtual Vector2& Get()             = 0;
    virtual const Vector2& Get() const = 0;
    virtual void Update()              = 0; // Vector3からVector2に値を同期
    virtual void Apply()               = 0; // Vector2からVector3に値を適用
};

// XY軸プロキシ
class XYProxy : public IVector2Proxy {
public:
    explicit XYProxy(Vector3* target);
    Vector2& Get() override;
    const Vector2& Get() const override;
    void Update() override;
    void Apply() override;

private:
    Vector3* target_;
    Vector2 proxy_;
};

// XZ軸プロキシ
class XZProxy : public IVector2Proxy {
public:
    explicit XZProxy(Vector3* target);
    Vector2& Get() override;
    const Vector2& Get() const override;
    void Update() override;
    void Apply() override;

private:
    Vector3* target_;
    Vector2 proxy_;
};

// YZ軸プロキシ
class YZProxy : public IVector2Proxy {
public:
    explicit YZProxy(Vector3* target);
    Vector2& Get() override;
    const Vector2& Get() const override;
    void Update() override;
    void Apply() override;

private:
    Vector3* target_;
    Vector2 proxy_;
};
