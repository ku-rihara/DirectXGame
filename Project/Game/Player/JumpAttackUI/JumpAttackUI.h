#pragma once
#include "2d/Sprite.h"
#include "Easing/Easing.h"
#include "Vector2.h"
#include <array>
#include <memory>

class ViewProjection;

/// <summary>
/// ジャンプ攻撃UI管理クラス
/// </summary>
class JumpAttackUI {
private:
    // ボタンタイプ
    enum Type {
        X,
        Y,
        B,
        A,
        COUNT
    };
    // 状態
    enum class State {
        WAIT,
        OPEN,
        CLOSE,
    };

public:
    struct BottomParam {
        std::unique_ptr<KetaEngine::Sprite> sprite;
        Vector2 discreteDirection;
        Vector2 posValueOffset;
        bool isNotOperate;
    };

    struct EasingParam {
        std::unique_ptr<KetaEngine::Easing<float>> posEasing;
        std::unique_ptr<KetaEngine::Easing<float>> rotateEasing;
        std::unique_ptr<KetaEngine::Easing<Vector2>> scaleEasing;
        float posEaseValue;
        float rotateEaseValue;
        Vector2 scaleEaseValue;
    };

public:
    JumpAttackUI()  = default;
    ~JumpAttackUI() = default;

    // 初期化
    void Init();

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="basePos">基準位置</param>
    /// <param name="viewProjection">ビュープロジェクション</param>
    void Update(const Vector3& basePos, const KetaEngine::ViewProjection& viewProjection);

    void RegisterParams(); //< パラメータのバインド
    void AdjustParam(); //< パラメータの調整
    void StartOpen(); //< 開くアニメーション開始
    void StartClose(); //< 閉じるアニメーション開始

private:
    /// <summary>
    /// ボタンごとの方向設定
    /// </summary>
    /// <param name="TypeNum">ボタンタイプ番号</param>
    void DirectionByBottom(const size_t& TypeNum);

    /// <summary>
    /// タイプに応じたボタン名取得
    /// </summary>
    /// <param name="TypeNum">ボタンタイプ番号</param>
    std::string BottomNameByType(const size_t& TypeNum);

    static void (JumpAttackUI::* spFuncTable_[])();

    void EasingInit(); //< イージングの初期化

    // state
    void Wait(); //< 待機状態
    void Open(); //< 開く状態
    void Close(); //< 閉じる状態

private:
    // global param
    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    const std::string groupName_      = "JumpAttackUI";

    // スプライト
    std::array<BottomParam, static_cast<size_t>(Type::COUNT)> bottoms_;
    State state_ = State::WAIT;
    EasingParam easingParam_;

    float notOperateAlpha_;

public:
};