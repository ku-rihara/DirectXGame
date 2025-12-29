#pragma once
#include "2d/Sprite.h"
#include "Easing/Easing.h"
#include "Vector2.h"
#include <memory>

/// <summary>
/// 操作方法表示クラス
/// </summary>
class HowToOperate {
public:
    HowToOperate()  = default;
    ~HowToOperate() = default;

    // 初期化、更新
    void Init();
    void Update();

    void ScalingEasing(); //< スケールイージング
    void Draw();          //< 描画
    void Debug();         //< デバッグ

private:
    // 座標
    Vector2 position_;
    // サイズ
    Vector2 size_;
    Vector2 scale_;
    // スプライト
    std::unique_ptr<KetaEngine::Sprite> sprite_;
    KetaEngine::Easing<Vector2> scaleEasing_;

    public:
    void SetScale();
};