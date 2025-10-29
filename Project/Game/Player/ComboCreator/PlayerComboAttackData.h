#pragma once

#include "Combo/Combo.h"
#include "Easing/Easing.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include "Vector3.h"
#include <cstdint>
#include <string>

class Combo;

/// <summary>
/// プレイヤー攻撃データクラス
/// </summary>
class PlayerComboAttackData {
public:
    // 移動パラメータ
    struct MoveParam {
        Vector3 value;
        int32_t easeType = 0;
        Easing<Vector3> easing;
    };

    // ノックバックパラメータ
    struct KnockBackParam {
        float speed;
        int32_t decreaseEaseType = 0;
        Easing<float> decreaseEase;
    };

    // コリジョンパラメータ
    struct CollisionParam {
        Vector3 collisionSize;
        float collisionOffsetValue;
        float adaptTime;
    };

    // アタックパラメータ
    struct AttackParameter {
        CollisionParam collisionPara;
        MoveParam moveParam;
        KnockBackParam knockBackParam;
        float cancelFrame;
        float precedeInputFrame;
        float power;
    };

public:
    PlayerComboAttackData()  = default;
    ~PlayerComboAttackData() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="attackName">攻撃名</param>
    void Init(const std::string& attackName);

    /// <summary>
    /// イージングタイプセレクター
    /// </summary>
    /// <param name="label">ラベル</param>
    /// <param name="target">対象変数</param>
    void EasingTypeSelector(const char* label, int32_t& target);

    void AdjustParam(); //< パラメータ調整
    void BindParams(); //< パラメータバインド
    void LoadData(); //< データロード
    void SaveData(); //< データセーブ

private:
    GlobalParameter* globalParameter_;
    std::string groupName_;
    const std::string folderPath_ = "AttackCreator";

    // 攻撃パラメータ
    AttackParameter attackParam_;

public:
    const std::string& GetGroupName() const { return groupName_; }
    AttackParameter& GetAttackParam() { return attackParam_; }
};