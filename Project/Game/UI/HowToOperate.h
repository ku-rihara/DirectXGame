#pragma once
#include "2d/Sprite.h"
#include "Easing/Easing.h"
#include "Vector2.h"
#include <memory>

class HowToOperate {
public:
    HowToOperate() = default;
    ~HowToOperate() = default;
    // 初期化
    void Init();
    // 更新
    void Update();
    void ScalingEasing();
    // 描画
    void Draw();
    void Debug();

private:
    // 座標
    Vector2 position_;
    // サイズ
    Vector2 size_;
    Vector2 scale_;
    // スプライト
    std::unique_ptr<Sprite> sprite_;
    Easing<Vector2> scaleEasing_;
};