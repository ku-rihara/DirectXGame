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

    void Draw(); //< 描画
    void Debug(); //< デバッグ

private:
    // 座標
    Vector2 position_;
    // サイズ
    Vector2 size_;
    // スプライト
    std::unique_ptr<KetaEngine::Sprite> sprite_;

public:
    void SetScale(const Vector2&scale);

};