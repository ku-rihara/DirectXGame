#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include <array>
#include <cstdint>
#include <memory>
#include <string>

class Player;

namespace KetaEngine {
class Sprite;
class ViewProjection;
} // namespace KetaEngine

/// <summary>
/// 次の攻撃候補をプレイヤー近傍の3D空間に投影して表示するUIクラス
/// </summary>
class NextAttackIndicatorUI {
public:
    // 対応するゲームパッドボタン数（GamepadButton::COUNT）
    static constexpr int32_t kButtonCount = 14;

    /// <summary>
    /// ボタンごとの表示設定
    /// </summary>
    struct ButtonConfig {
        Vector3 worldOffset = {0.0f, 2.0f, 0.0f}; //< プレイヤー原点からのワールドオフセット
        Vector2 spriteScale = {64.0f, 64.0f};       //< スプライトのスケール(px)
        float alpha         = 1.0f;                 //< 不透明度
        std::string textureName;                    //< テクスチャファイル名
    };

public:
    NextAttackIndicatorUI()  = default;
    ~NextAttackIndicatorUI() = default;

    /// <summary>
    /// 初期化。各ボタンのデフォルトテクスチャを設定してスプライトを生成する
    /// </summary>
    void Init(Player* player, const KetaEngine::ViewProjection* viewProjection);

    /// <summary>
    /// 毎フレーム更新。現在の次攻撃候補を取得しスプライト位置を更新する
    /// </summary>
    void Update();

    /// <summary>
    /// スプライトを描画する
    /// </summary>
    void Draw();

    /// <summary>
    /// ImGuiによるパラメータ調整UI
    /// </summary>
    void DrawEditorUI();

private:
    Vector2 WorldToScreen(const Vector3& worldPos) const;

private:
    Player*                          pPlayer_         = nullptr;
    const KetaEngine::ViewProjection* pViewProjection_ = nullptr;

    struct IndicatorEntry {
        ButtonConfig                      config;
        std::unique_ptr<KetaEngine::Sprite> sprite;
    };

    std::array<IndicatorEntry, kButtonCount> entries_;

    static constexpr float kScreenWidth  = 1280.0f;
    static constexpr float kScreenHeight = 720.0f;
};
