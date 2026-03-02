#pragma once
//2D
#include "2d/Sprite.h"
// Easing
#include "Easing/Easing.h"
// math
#include "Vector2.h"
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
       /* std::unique_ptr<KetaEngine::Sprite> outLineSprite_;
        std::unique_ptr<KetaEngine::Sprite> uiFrame;*/
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

private:
    // ボタンごとのスプライトファイルパス
    const std::string kButtonTextures[static_cast<int32_t>(OperateButtonType::COUNT)] = {
        "OperateUI/Operate_X.dds",
        "OperateUI/Operate_Y.dds",
        "OperateUI/Operate_B.dds",
        "OperateUI/Operate_A.dds",
        "OperateUI/Operate_LB.dds",
    };

    // アウトラインは共通

    Player* player_ = nullptr;
    std::array<UIParam, static_cast<int32_t>(OperateButtonType::COUNT)> uiParam_;

public:
    void SetScale(const Vector2& scale);
    void SetPlayer(Player* player) { player_ = player; }
};