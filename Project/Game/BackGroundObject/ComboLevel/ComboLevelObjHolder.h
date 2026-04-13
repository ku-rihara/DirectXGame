#pragma once
#include "BaseComboLevelBackObj.h"
#include <memory>
#include <string>
#include <vector>

/// <summary>
/// コンボレベルオブジェクトの保持、管理クラス
/// </summary>
class ComboLevelObjHolder {
public:
    ComboLevelObjHolder()  = default;
    ~ComboLevelObjHolder() = default;

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="playSpeed">再生速度</param>
    void Update(float playSpeed);

    /// <summary>
    /// オブジェクトの追加
    /// </summary> 
    /// <param name="filename">Blenderシーンファイル名</param>
    /// <param name="comboLevel">出現するコンボレベル</param>
    /// <param name="isPulseMaster">PULSEタイミングの基準オブジェクトか</param>
    void Add(const std::string& filename, int32_t comboLevel, bool isPulseMaster = false);

    /// <summary>
    /// 指定コンボレベル以下のオブジェクトをCloseモードに設定
    /// </summary>
    /// <param name="level">コンボレベル</param>
    void CloseForComboLevel(int32_t level);

    /// <summary>
    /// 指定コンボレベルのエフェクトモードを設定
    /// </summary>
    /// <param name="comboLevel">コンボレベル</param>
    /// <param name="mode">エフェクトモード</param>
    void SetEffectMode(int32_t comboLevel, const ObjEffectMode& mode);

    void EasingResetSelectGroup(int32_t groupNum); //< 指定グループのイージングリセット
    void EasingAllReset();                         //< 全イージングリセット

    bool GetIsEasingFinish(int32_t comboLevel, int32_t groupNum) const; //< イージング完了フラグ取得

private:
    struct Entry {
        std::unique_ptr<BaseComboLevelBackObj> obj;
        int32_t comboLevel = 0;
    };

    std::vector<Entry> entries_;
    BaseComboLevelBackObj* pulseMaster_ = nullptr;
};
