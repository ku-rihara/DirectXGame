#pragma once

/// utility
#include "Editor/ParameterEditor/GlobalParameter.h"

/// std
#include <string>

/// アッパー攻撃パラメータ
struct UpperParam {
    float BackLashValue; ///< 反動量
    float chargeTime;    ///< チャージ最大時間
};

/// ジャンプ・落下パラメータ
struct JumpParam {
    float jumpSpeed;      ///< 初速
    float gravity;        ///< 重力加速度
    float fallSpeedLimit; ///< 落下速度上限
};

/// コンボ継続パラメータ
struct ComboParam {
    float waitTime;    ///< 次入力受付時間
    float attackReach; ///< 攻撃リーチ
};

/// スポーンパラメータ
struct SpawnParam {
    float waitTime_; ///< スポーン演出待機時間
};

/// ダッシュパラメータ
struct DashParam {
    float distance = 20.0f; ///< ダッシュ移動距離
};

/// プレイヤー全パラメータ
struct Parameters {
    Vector3 startPos_;          ///< 初期配置座標
    Vector3 baseScale_;         ///< 基準スケール
    float rushDistance;         ///< ラッシュ移動距離
    float upperPosY;            ///< アッパー時の打ち上げ高さ
    UpperParam upperParam;      ///< アッパー攻撃
    JumpParam normalJump;       ///< 通常ジャンプ
    JumpParam boundJump;        ///< バウンドジャンプ
    JumpParam upperJump;        ///< アッパー後ジャンプ
    SpawnParam spawnParam;      ///< スポーン
    float moveSpeed;            ///< 移動速度
    float dashSpeedMultiplier;  ///< ダッシュ速度倍率
    DashParam dashParam;        ///< ダッシュ
    float fallSpeed;            ///< 落下速度
    float attackRotate;         ///< 攻撃時の回転量
    float attackRotateAnit;     ///< 攻撃回転の逆方向量
    float attackFloatValue;     ///< 攻撃中の浮遊量
};


/// <summary>
/// プレイヤーパラメータ管理クラス
/// GlobalParameter を通じて JSON からパラメータを読み込む
/// </summary>
class PlayerParameter {
public:
    PlayerParameter()  = default;
    ~PlayerParameter() = default;

    /// ===================================================
    ///  public method
    /// ===================================================

    void Init();
    void RegisterParams(); ///< GlobalParameter へのパラメータバインド
    void AdjustParam();    ///< ImGui によるパラメータ調整

private:
    KetaEngine::GlobalParameter* globalParameter_;
    const std::string groupName_ = "Player";

private:
    Parameters playerParams_;

public:
    const Parameters& GetParameters() const { return playerParams_; }
};
