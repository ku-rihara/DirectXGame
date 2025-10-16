#pragma once

#include "3d/WorldTransform.h"
#include "Collider/AABBCollider.h"
#include <string>

class BaseAABBCollisionBox : public AABBCollider {
public:
    BaseAABBCollisionBox()          = default;
    virtual ~BaseAABBCollisionBox() = default;

    /// ====================================================
    ///  public method
    /// ====================================================

    virtual void Init();
    virtual void Update();
    virtual void Draw();

   
    virtual void SetParentTransform(WorldTransform* transform);

    // コールバック関数
    virtual void OnCollisionEnter([[maybe_unused]] BaseCollider* other) override;
    virtual void OnCollisionStay([[maybe_unused]] BaseCollider* other) override;

protected:
    // transform,offset
    WorldTransform transform_;
    Vector3 offset_;
    Vector3 size_;
    bool isCollision_ = true;

public:
    std::string tag_;

public:
    /// getter
    virtual Vector3 GetCollisionPos() const override;
    const std::string& GetTag() const { return tag_; }

    /// setter
    void SetIsCollision(const bool& is) { isCollision_ = is; } 
    void SetSize(const Vector3& size) { size_ = size; }
    void SetPosition(const Vector3& position) { transform_.translation_ = position; }
    void SetOffset(const Vector3& offset) { offset_ = offset;}
};