#pragma once

#include "Combo/ComboUIController.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
// std
#include <cstdint>
#include <memory>

constexpr int32_t kComboLevel = 5;

/// <summary>
/// コンボシステムの管理クラス
/// </summary>
class Combo {
public:
    Combo()  = default;
    ~Combo() = default;

    // 初期化、更新
    void Init();
    void Update();
   
    void ComboCountUP();        //< コンボカウント増加
    void ComboTimerDecrement(); //< コンボタイマー減少
    void LevelUp();             //< レベルアップ
    void Reset();               //< リセット

    void AdjustParam(); //< パラメータ調整
    void RegisterParams(); //< パラメータバインド

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
    int32_t GetComboCount() const { return comboCount_; }
    int32_t GetCurrentLevel() const { return currentLevel_; }
    int32_t GetLevelUPNum(int32_t level) const { return LevelUpNum[level]; }
    const bool& GetIsReset() const { return isReset_; }
};