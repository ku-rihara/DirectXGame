#pragma once
#include "../BaseBackGroundObject.h"
#include <cstdint>
#include <string>

enum class ObjEffectMode {
    SPAWN,
    PULSE,
    CLOSE,
    NONE,
};

/// <summary>
/// コンボレベルに応じた背景オブジェクトの基底クラス
/// </summary>
class BaseComboLevelBackObj : public BaseBackGroundObject {
public:
    BaseComboLevelBackObj()          = default;
    virtual ~BaseComboLevelBackObj() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="filename">ファイル名</param>
    virtual void Init(const std::string& filename) override;

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="playSpeed">再生速度</param>
    virtual void Update(const float& playSpeed) override;

    /// <summary>
    /// 指定グループのイージングリセット
    /// </summary>
    /// <param name="groupNum">グループ番号</param>
    void EasingResetSelectGroup(const int32_t& groupNum);

    /// <summary>
    /// イージング完了フラグ取得
    /// </summary>
    /// <param name="groupNum">グループ番号</param>
    bool GetIsEasingFinish(const int32_t& groupNum) const;

    void EasingAllReset(); //< 全イージングリセット

protected:
    ObjEffectMode effectMode_ = ObjEffectMode::NONE;
    bool isPulseCycleStart_   = false;

public:
    void SetEffectMode(const ObjEffectMode& mode) { effectMode_ = mode; }

    /// <summary>
    /// エフェクトモードを整数に変換
    /// </summary>
    /// <param name="mode">エフェクトモード</param>
    int32_t ConvertEffectModeToInt(const ObjEffectMode& mode);

    void SetIsPulseCycleStart(const bool& isStart) { isPulseCycleStart_ = isStart; }
};