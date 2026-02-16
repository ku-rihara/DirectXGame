#pragma once
#include "Combo/Combo.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "EnemyDamageReactionData.h"
#include "utility/FileSelector/FileSelector.h"
#include <array>
#include <memory>
#include <string>
#include <vector>

/// <summary>
/// デフォルトアニメーションの種類
/// </summary>
enum class DefaultAnimType {
    Normal = 0,  // 通常ダメージリアクション
    TakeUpper,   // 上に飛ばされた時
    Slammed,     // 叩きつけ
    Bound,       // バウンド中
    GetUp,       // 起き上がり
    Count
};

/// <summary>
/// 敵のダメージリアクション
/// </summary>
class EnemyDamageReactionController {
public:
    EnemyDamageReactionController() = default;
    ~EnemyDamageReactionController() = default;

    //*-------------------------------- public Method --------------------------------*//

    // 初期化、更新
    void Init();
    void Update(float deltaTime);

    // 攻撃データの追加、削除
    void AddAttack(const std::string& attackName);
    void RemoveAttack(const int& index);

    void EditorUpdate(); //< エディタ更新
    void AllLoadFile(); //< 全ファイルロード
    void AllSaveFile(); //< 全ファイルセーブ

private:

    //*-------------------------------- Private variants--------------------------------*//
    
    // 共通パラメータ
    const std::string AttackDataFolderPath_ = "Resources/GlobalParameter/EnemyDamageReaction/ReactionDates/";

    std::vector<std::unique_ptr<EnemyDamageReactionData>> reactions_;
    int selectedIndex_    = -1;
    char nameBuffer_[128] = "";

    // デフォルトパラメータ（敵タイプ別: 0=Normal, 1=Strong）
    static constexpr int kEnemyTypeCount = 2;
    static constexpr int kDefaultAnimTypeCount = static_cast<int>(DefaultAnimType::Count);
    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    const std::string defaultParamGroupName_ = "EnemyDamageReaction_Default";
    const std::string defaultParamFolderPath_ = "EnemyDamageReaction";
    // [敵タイプ][アニメーション種類]
    std::array<std::array<std::string, kDefaultAnimTypeCount>, kEnemyTypeCount> defaultDamageAnimationNames_;
    // イージング系は敵タイプ別のみ
    std::array<std::string, kEnemyTypeCount> defaultObjEaseAnimationNames_;
    std::array<float, kEnemyTypeCount> defaultObjEaseAnimationStartTimings_ = {0.0f, 0.0f};
    std::array<KetaEngine::FileSelector, kEnemyTypeCount> defaultObjEaseFileSelectors_;

public:
    EnemyDamageReactionData* GetSelectedAttack();
    EnemyDamageReactionData* GetAttackByTriggerName(const std::string& name);
    const std::vector<std::unique_ptr<EnemyDamageReactionData>>& GetAllAttacks() const { return reactions_; }
    const int& GetAttackCount() const { return static_cast<int>(reactions_.size()); }

    // デフォルトパラメータのGetter（enemyType: 0=Normal, 1=Strong）
    const std::string& GetDefaultDamageAnimationName(int enemyType, DefaultAnimType animType) const {
        return defaultDamageAnimationNames_[enemyType][static_cast<int>(animType)];
    }
    const std::string& GetDefaultObjEaseAnimationName(int enemyType) const { return defaultObjEaseAnimationNames_[enemyType]; }
    float GetDefaultObjEaseAnimationStartTiming(int enemyType) const { return defaultObjEaseAnimationStartTimings_[enemyType]; }
 };