#pragma once
#include "Combo/Combo.h"
#include "PlayerComboAttackData.h"
#include <array>
#include <memory>
#include <string>
#include <vector>

/// <summary>
/// コンボアタック制御
/// </summary>
class PlayerComboAttackController {
public:
    struct AttackValueForLevel {
        float speedRate;
        float powerRate;
    };

public:
    PlayerComboAttackController()  = default;
    ~PlayerComboAttackController() = default;

    //*-------------------------------- public Method --------------------------------*//

    // 初期化、更新
    void Init();
    void Update(const float& deltaTime);

    // 攻撃データの追加、削除
    void AddAttack(const std::string& attackName);
    void RemoveAttack(const int& index);

    void EditorUpdate(); //< エディタ更新
    void AllLoadFile(); //< 全ファイルロード
    void AllSaveFile(); //< 全ファイルセーブ

private:

    //*-------------------------------- private Method --------------------------------*//
    void BindCommonParams(); //< 共通パラメータバインド
    void AdjustCommonParam(); //< 共通パラメータ調整

private:

    //*-------------------------------- Private variants--------------------------------*//
    
    // 共通パラメータ
    GlobalParameter* globalParameter_;
    const std::string commonGroupName_      = "ComboCommonParam";
    const std::string AttackDataFolderPath_ = "Resources/GlobalParameter/AttackCreator/";

    std::vector<std::unique_ptr<PlayerComboAttackData>> attacks_;
    int selectedIndex_    = -1;
    char nameBuffer_[128] = "";

    std::array<AttackValueForLevel, kComboLevel> attackValueForLevel_;

public:
    PlayerComboAttackData* GetSelectedAttack();
    PlayerComboAttackData* GetAttackByName(const std::string& name);
    const int& GetAttackCount() const { return static_cast<int>(attacks_.size()); }
    const std::array<AttackValueForLevel, kComboLevel>& GetAttackValueForLevel() const { return attackValueForLevel_; }
};