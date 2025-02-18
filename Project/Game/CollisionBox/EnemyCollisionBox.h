#pragma once

#include"BaseAABBCollisionBox.h"
#include<array>

class EnemyCollisionBox :public BaseAABBCollisionBox {

private:
    ///------------------------------------------------------------------------------
    /// private variant
    ///------------------------------------------------------------------------------

public:

    ///------------------------------------------------------------------------------
    /// public method
    ///------------------------------------------------------------------------------

    void Init()override;
    void Update()override;
    void Draw()override;

    void OnCollisionStay([[maybe_unused]] BaseCollider* other)override;
    void OnCollisionEnter([[maybe_unused]] BaseCollider* other)override;

    ///------------------------------------------------------------------------------
    /// setter method
    ///------------------------------------------------------------------------------

    void IsAdapt(bool is)override;
    void SetSize(const Vector3& size)override;
    void SetPosition(const Vector3& position)override;
    void SetOffset(const Vector3& offset)override;

    ///------------------------------------------------------------------------------
    /// setter method
    ///------------------------------------------------------------------------------

    Vector3 GetCollisionPos() const override;

};