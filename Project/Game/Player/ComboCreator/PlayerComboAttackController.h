#pragma once
#include "Combo/Combo.h"
#include "PlayerComboAttackData.h"
#include <array>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

class Combo;

namespace KetaEngine {
class EffectEditorSuite;
}

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
   
    // 攻撃データの追加、削除
    void AddAttack(const std::string& attackName);
    void RemoveAttack(const int& index);

    void EditorUpdate(); //< エディタ更新
    void AllLoadFile();  //< 全ファイルロード
    void AllSaveFile();  //< 全ファイルセーブ

    float GetRealAttackSpeed(float baseTimeSpeed) const;
    float GetPowerRate() const;

private:
    //*-------------------------------- private Method --------------------------------*//
    void BindCommonParams();  //< 共通パラメータバインド
    void AdjustCommonParam(); //< 共通パラメータ調整

    // コンボフロー可視化、チェーン構築
    void VisualizeComboFlow();
    std::vector<std::vector<std::string>> BuildComboChains();

    void BuildChainRecursive(
        const std::string& attackName,
        std::vector<std::string>& chain,
        std::unordered_set<std::string>& visited);

    // 攻撃名で選択
    void SelectAttackByName(const std::string& name);
    bool IsValidIndex(int index) const;

private:
    //*-------------------------------- Private variants--------------------------------*//

    // 共通パラメータ
    KetaEngine::GlobalParameter* globalParameter_;
    KetaEngine::EffectEditorSuite* pEditorSuite_;

    const std::string commonGroupName_      = "ComboCommonParam";
    const std::string AttackDataFolderPath_ = "Resources/GlobalParameter/AttackCreator/";

    std::vector<std::unique_ptr<PlayerComboAttackData>> attacks_;
    int selectedIndex_    = -1;
    char nameBuffer_[128] = "";

    std::array<AttackValueForLevel, kComboLevel> attackValueForLevel_;
    Combo* pCombo_ = nullptr;

public:
    PlayerComboAttackData* GetSelectedAttack();
    PlayerComboAttackData* GetAttackByName(const std::string& name);
    const std::vector<std::unique_ptr<PlayerComboAttackData>>& GetAllAttacks() const { return attacks_; }
    const int& GetAttackCount() const { return static_cast<int>(attacks_.size()); }

    void SetEditorSuite(KetaEngine::EffectEditorSuite* editorSuite);
    void SetCombo(Combo* combo);
};