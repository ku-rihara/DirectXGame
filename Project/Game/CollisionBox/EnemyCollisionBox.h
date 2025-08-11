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

   

    ///------------------------------------------------------------------------------
    /// setter method
    ///------------------------------------------------------------------------------

    Vector3 GetCollisionPos() const override;

};