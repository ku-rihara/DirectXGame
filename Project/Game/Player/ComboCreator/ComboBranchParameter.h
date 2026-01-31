#pragma once

#include "Editor/ParameterEditor/GlobalParameter.h"
#include "utility/FileSelector/FileSelector.h"

#include <cstdint>
#include <string>

class PlayerComboAttackData;

/// <summary>
/// コンボ分岐パラメータクラス
/// </summary>
class ComboBranchParameter {
public:
    ComboBranchParameter()  = default;
    ~ComboBranchParameter() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="groupName">グループ名</param>
    /// <param name="branchIndex">分岐インデックス</param>
    void Init(const std::string& groupName, int32_t branchIndex);

    /// <summary>
    /// パラメータ登録
    /// </summary>
    void RegisterParams();

    /// <summary>
    /// パラメータ設定UIの描画
    /// </summary>
    /// <param name="attackFolderPath">攻撃データフォルダパス</param>
    /// <param name="parentGroupName">親の攻撃グループ名（FileSelector用）</param>
    /// <returns>削除が要求された場合true</returns>
    bool DrawParameterUI(const std::string& attackFolderPath, const std::string& parentGroupName);

private:
    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    KetaEngine::FileSelector fileSelector_;

    std::string groupName_;
    int32_t branchIndex_ = 0;

    // パラメータ
    std::string nextAttackName_;
    int32_t keyboardButton_ = 0;
    int32_t gamepadButton_  = 0;
    float cancelTime_       = 0.0f;
    float precedeInputTime_ = 0.0f;
    bool requireHit_        = false;

public:
    // Getter
    const std::string& GetNextAttackName() const { return nextAttackName_; }
    int32_t GetKeyboardButton() const { return keyboardButton_; }
    int32_t GetGamepadButton() const { return gamepadButton_; }
    float GetCancelTime() const { return cancelTime_; }
    float GetPrecedeInputTime() const { return precedeInputTime_; }
    bool GetRequireHit() const { return requireHit_; }

    // Setter
    void SetCancelTime(float time) { cancelTime_ = time; }
    void SetPrecedeInputTime(float time) { precedeInputTime_ = time; }
};