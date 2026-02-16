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

    // デフォルトパラメータ
    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    const std::string defaultParamGroupName_ = "EnemyDamageReaction_Default";
    const std::string defaultParamFolderPath_ = "EnemyDamageReaction";
    std::string defaultDamageAnimationName_;
    std::string defaultObjEaseAnimationName_;
    float defaultObjEaseAnimationStartTiming_ = 0.0f;
    KetaEngine::FileSelector defaultObjEaseFileSelector_;

public:
    EnemyDamageReactionData* GetSelectedAttack();
    EnemyDamageReactionData* GetAttackByTriggerName(const std::string& name);
    const std::vector<std::unique_ptr<EnemyDamageReactionData>>& GetAllAttacks() const { return reactions_; }
    const int& GetAttackCount() const { return static_cast<int>(reactions_.size()); }

    // デフォルトパラメータのGetter
    const std::string& GetDefaultDamageAnimationName() const { return defaultDamageAnimationName_; }
    const std::string& GetDefaultObjEaseAnimationName() const { return defaultObjEaseAnimationName_; }
    float GetDefaultObjEaseAnimationStartTiming() const { return defaultObjEaseAnimationStartTiming_; }
 };