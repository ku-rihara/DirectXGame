#pragma once
#include "BaseComboLevelBackObj.h"
#include <array>
#include <memory>
#include <string>

enum class ComboLevelObjType {
    STADIUM_LIGHT,
    SPEAKER,
    COUNT
};

/// <summary>
/// コンボレベルオブジェクトの保持、管理クラス
/// </summary>
class ComboLevelObjHolder {
public:
    ComboLevelObjHolder()  = default;
    ~ComboLevelObjHolder() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="filename">ファイル名</param>
    void Init(const std::string& filename);

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="playSpeed">再生速度</param>
    void Update(float playSpeed);

    /// <summary>
    /// 指定グループのイージングリセット
    /// </summary>
    /// <param name="groupNum">グループ番号</param>
    void EasingResetSelectGroup(int32_t groupNum);

    /// <summary>
    /// コンボレベルに応じたクローズ処理
    /// </summary>
    /// <param name="level">コンボレベル</param>
    void CloseForComboLevel(int32_t level);

    /// <summary>
    /// オブジェクトの追加
    /// </summary>
    /// <param name="type">オブジェクトタイプ</param>
    /// <param name="filename">ファイル名</param>
    void Add(const ComboLevelObjType& type, const std::string& filename);

    /// <summary>
    /// エフェクトモードの設定
    /// </summary>
    /// <param name="type">オブジェクトタイプ</param>
    /// <param name="mode">エフェクトモード</param>
    void SetEffectMode(int32_t level, const ObjEffectMode& mode);

    void EasingAllReset(); //< 全イージングリセット

private:
    std::array<std::unique_ptr<BaseComboLevelBackObj>, static_cast<size_t>(ComboLevelObjType::COUNT)> objects_;

public:
    /// <summary>
    /// イージング完了フラグ取得
    /// </summary>
    /// <param name="type">オブジェクトタイプ</param>
    /// <param name="groupNum">グループ番号</param>
    bool GetIsEasingFinish(const ComboLevelObjType& type, int32_t groupNum) const;
    size_t ToIndex(const ComboLevelObjType& type) const { return static_cast<size_t>(type); }
};