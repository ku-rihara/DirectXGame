#pragma once

#include "2d/Sprite.h"
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
        Count,
    };

public:
    ComboUI()  = default;
    ~ComboUI() = default;

    // init update draw
    void Init(const ComboDigit& digit);
    void Update(const Vector2& scale, const float& alpha);
    void Draw();

    void CalculateNumber(const int32_t& value);
    void CreateGroupName(const ComboDigit&digit);
  
   
    ///-------------------------------------------------------------------------------------
    /// Editor
    ///-------------------------------------------------------------------------------------
    void AdjustParam();
    void BindParams();

private:
    ///* globalParameter *//
    GlobalParameter* globalParameter_; 
    std::string groupName_; 

    ///* parameter *//
    ComboDigit comboDigit_ = ComboDigit::ONE; 
    Vector2 position_;
    int32_t valueForDigit_;
    float uvPosX_;
    float uvScaleOffset_;

    //* variables *//
    bool isVisible_; 

    ///* Sprite *//
    std::unique_ptr<Sprite> sprite_;

public: 

    ///* Getter *//
   const ComboDigit& GetComboDigit() const { return comboDigit_; }

    ///* Setter *//
   void SetComboDigit(const ComboDigit& digit) { comboDigit_ = digit; }
};
