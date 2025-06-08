#pragma once

#include "2d/Sprite.h"
#include "Easing.h"
#include "utility/ParameterEditor/GlobalParameter.h"
//std
#include <cstdint>
#include <memory>

class ComboUIController {

public:
    ComboUIController() = default;
    ~ComboUIController() = default;

    // init update draw
    void Init();
    void Update();
    void Draw();

    ///-------------------------------------------------------------------------------------
    /// Editor
    ///-------------------------------------------------------------------------------------
    void AdjustParm();
    void BindParams();

private:
    GlobalParameter* globalParameter_; /// グローバルパラメータ
    const std::string groupName_ = "ComboUI"; /// グループ名

    ///* parameter *//
    Easing scalingEasing_;

    ///* Sprite *//
    std::unique_ptr<Sprite> sprite_;
};
