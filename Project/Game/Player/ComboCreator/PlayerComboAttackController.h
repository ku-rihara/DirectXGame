#pragma once
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

    void Init(); //< 初期化

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    void Update(const float& deltaTime);

    /// <summary>
    /// 攻撃データの追加
    /// </summary>
    /// <param name="attackName">攻撃名</param>
    void AddAttack(const std::string& attackName);

    /// <summary>
    /// 攻撃データの削除
    /// </summary>
    /// <param name="index">インデックス</param>
    void RemoveAttack(const int& index);

    void EditorUpdate(); //< エディタ更新
    void AllLoadFile(); //< 全ファイルロード
    void AllSaveFile(); //< 全ファイルセーブ

    PlayerComboAttackData* GetSelectedAttack(); //< 選択中の攻撃データ取得

    /// <summary>
    /// 名前から攻撃データを取得
    /// </summary>
    /// <param name="name">攻撃名</param>
    /// <returns>攻撃データ</returns>
    PlayerComboAttackData* GetAttackByName(const std::string& name);

private:
    void BindCommonParams(); //< 共通パラメータバインド
    void AdjustCommonParam(); //< 共通パラメータ調整

private:
    std::vector<std::unique_ptr<PlayerComboAttackData>> attacks_;

    Vector3 basePos_   = Vector3::ZeroVector();
    int selectedIndex_ = -1;

    bool isPreViewDraw_;

    char nameBuffer_[128] = "";

    // 共通パラメータ
    GlobalParameter* globalParameter_;
    const std::string commonGroupName_ = "ComboCommonParam";

    const std::string AttackDataFolderPath_ = "Resources/GlobalParameter/AttackCreator/";

    std::array<AttackValueForLevel, kComboLevel> attackValueForLevel_;

public:
    const int& GetAttackCount() const { return static_cast<int>(attacks_.size()); }
    const std::array<AttackValueForLevel, kComboLevel>& GetAttackValueForLevel() const { return attackValueForLevel_; }
};