#pragma once

#include "2d/Sprite.h"
#include "Easing.h"
#include "utility/ParameterEditor/GlobalParameter.h"
//std
#include <cstdint>
#include <memory>

class Combo {

public:
    Combo()  = default;
    ~Combo() = default;

    // init update draw
    void Init();
    void Update();
    void Draw();

    ///-------------------------------------------------------------------------------------
    /// Editor
    ///-------------------------------------------------------------------------------------
    void ParamLoadForImGui();
    void AddParmGroup();
    void SetValues();
    void ApplyGlobalParameter();
    void AdjustParm();

    // new 
    void BindParams();

private:
    GlobalParameter* globalParameter_; /// グローバルパラメータ
    const std::string groupName_ = "Combo"; /// グループ名

    ///* parameter *//
    float comboTime_; /// コンボ時間
    int32_t comboCount_; /// コンボ数
    float comboTimeMax_; /// コンボ時間の最大値

    ///* Reaction parameter *//
    Easing scalingEasing_;

    ///* Sprite *//
    std::unique_ptr<Sprite> sprite_;
};
