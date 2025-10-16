#pragma once

#include "Combo/ComboUIController.h"
#include "utility/ParameterEditor/GlobalParameter.h"
// std
#include <cstdint>
#include <memory>

constexpr int32_t kComboLevel = 5;

class Combo {
public:
    Combo()  = default;
    ~Combo() = default;

    // init update draw
    void Init();
    void Update();
    void Draw();

    void ComboCountUP();
    void ComboTimerDecrement();
    void LevelUp();
    void Reset();

    ///-------------------------------------------------------------------------------------
    /// Editor
    ///-------------------------------------------------------------------------------------
    void AdjustParam();
    void BindParams();

private:
    // UIコントローラ
    std::unique_ptr<ComboUIController> uiController_;

    GlobalParameter* globalParameter_;
    const std::string groupName_ = "Combo";

    // parameter
    float comboTime_;
    int32_t comboCount_;
    float comboTimeMax_;
    int32_t LevelUpNum[kComboLevel];
    int32_t currentLevel_;

    bool isReset_ = false;

public: 
    // Getter
    const int32_t& GetComboCount() const { return comboCount_; }
    const int32_t& GetCurrentLevel() const { return currentLevel_; }
    const int32_t& GetLevelUPNum(const int32_t& level) const { return LevelUpNum[level]; }
    const bool& GetIsReset() const { return isReset_; }
};
