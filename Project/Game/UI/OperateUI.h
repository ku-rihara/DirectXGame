#pragma once
//2D
#include "2d/Sprite.h"
// Easing
#include "Easing/Easing.h"
// Editor
#include "Editor/ParameterEditor/GlobalParameter.h"
// math
#include "Vector2.h"
#include "Vector3.h"
// std
#include <array>
#include <cstdint>
#include <memory>
#include <string>

class Player;

/// <summary>
/// 操作方法表示クラス
/// </summary>
class OperateUI {
public:
    enum class OperateButtonType {
        X,
        Y,
        B,
        A,
        LB,
        COUNT,
    };

    struct UIParam {
        std::unique_ptr<KetaEngine::Sprite> sprite_;
    };

public:
    OperateUI()  = default;
    ~OperateUI() = default;

    // 初期化、更新
    void Init();
    void Update();

    void Debug(); //< デバッグ
private:
    bool IsPushCondition(OperateButtonType type);
    void RegisterParams();

private:
    // ボタンごとのスプライトファイルパス
    const std::string kButtonTextures[static_cast<int32_t>(OperateButtonType::COUNT)] = {
        "OperateUI/Operate_X.dds",
        "OperateUI/Operate_Y.dds",
        "OperateUI/Operate_B.dds",
        "OperateUI/Operate_A.dds",
        "OperateUI/Operate_LB.dds",
    };

    Player* player_ = nullptr;
    std::array<UIParam, static_cast<int32_t>(OperateButtonType::COUNT)> uiParam_;

    // GlobalParameter
    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    const std::string groupName_                  = "OperateUI_Reaction";

    // 現在の基準スケール（SetScale で外部から設定される）
    Vector2 currentScale_;

    // 押下時スケール倍率（共通）
    float pressScaleMultiplier_ = 0.85f;

    // 押下時の色（ボタンごとに個別）
    std::array<Vector3, static_cast<int32_t>(OperateButtonType::COUNT)> pressColors_;

public:
    void SetScale(const Vector2& scale);
    void SetPlayer(Player* player) { player_ = player; }
};
