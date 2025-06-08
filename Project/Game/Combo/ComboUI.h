#pragma once

#include "2d/Sprite.h"
#include "Easing.h"
#include "utility/ParameterEditor/GlobalParameter.h"
// std
#include <cstdint>
#include <memory>

class ComboUI {
public:
    enum class ComboDigit {
        ONE,
        TWO,
        THREE,
    };

public:
    ComboUI()  = default;
    ~ComboUI() = default;

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
    ComboDigit comboDigit_ = ComboDigit::ONE; /// コンボの桁数

    ///* Sprite *//
    std::unique_ptr<Sprite> sprite_;

public: // accessors

    ///* Getter *//
    ComboDigit GetComboDigit() const { return comboDigit_; }

    ///* Setter *//
    void SetComboDigit(ComboDigit digit) { comboDigit_ = digit; }

};
